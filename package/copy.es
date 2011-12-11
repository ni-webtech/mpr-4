/*
    Install files support routines
 */

var masterOptions

const StandardFilter = /\.makedep$|\.o$|\.pdb$|\.tmp$|\.save$|\.sav$|OLD|\/Archive\/|\/sav\/|\/save\/|oldFiles|\.libs\/|\.nc|\.orig|\.svn|\.git|^\.[a-zA-Z_]|\.swp$|\.new$|\.nc$|.DS_Store/

/*
    copy files
    @param from Source path
    @param target Destination path
    @param options Copy options
    @option exclude Exclude files that match the pattern
    option include Include files that match the pattern
    option fold Fold long lines on windows and convert new line endings
    option zip Compress target file
    option owner Set file owner
    option group Set file group
    option perms Set file perms
    option strip Strip object or executable
    option expand Expand variables using %%TOKENS%%
    option trace Set to true to trace copies
 */
public function copy(src: Path, target: Path = Dir, options = {}) 
{
    blend(options, masterOptions)
    let copied = 0
    let group = options.group || -1
    let owner = options.owner || -1
    let permissions = options.permissions
    let task = options.task 
    let verbose: Boolean = options.trace == "1" || options.trace == true
    let compress = options.compress || false
    let build = options.build
    let log = App.log
    options.root ||= "/tmp"

    if (verbose) log.activity("Process", "cpy: " + src + " " + target)

    let pat = src.basename
    let dir, from
    if (options.from) {
        from = Path(options.from).relative
        dir = from.join(src.dirname)
    } else {
        dir = src.dirname
        from = ""
    }
    let files = options.top.join(dir).find(pat, options)

    for each (let file: Path in files) {
        file = file.relative.portable
        if (options.exclude && file.match(options.exclude)) {
            continue
        }
        if (file.match(StandardFilter)) {
            continue
        }
        if (options.include && !file.match(options.include)) {
            continue
        }
        let dest: Path
        if (target.isDir) {
            dest = Path("" + target + "/" + file.trimStart(from)).normalize.portable
        } else {
            dest = target
        }
        if (file.isDir) {
            dest.makeDir()
            continue
        }
        copied++
        if (task == "Remove") {
            if (compress) {
                dest = Path("" + dest.name + ".gz")
            }
            if (verbose) log.activity("Remove", dest)
            dest.remove()
        } else {
            let parent = dest.parent.relative.portable
            if (!dest.parent.isDir) {
                if (verbose) log.activity("MakeDir", parent)
                parent.makeDir({ group: group, owner: owner, permissions: 0755})
            }
            if (verbose) log.activity("Copy", dest.relative)
            permissions ||= file.extension.match(/exe|lib|so|dylib|sh|ksh/) ? 0755 : 0644
            let attributes = { permissions: permissions, owner: owner, group: group}
            file.copy(dest, attributes)

            if (options.expand) {
                if (verbose) log.activity("Patch", dest)
                expand(dest, options)
                dest.attributes = attributes
            }
            if (options.fold && Config.OS == "WIN") {
                fold(dest, options)
                dest.attributes = attributes
            }
            if (options.strip && build.BLD_STRIP != "" && build.BLD_UNIX_LIKE == 1 && build.BLD_BUILD_OS != "MACOSX") {
                if (verbose) {
                    log.activity("Strip", dest)
                }
                Cmd.sh(build.BLD_STRIP + " " + dest)
            }
            if (compress) {
                let destZip: Path = Path("" + dest.name + ".gz")
                destZip.remove()
                if (verbose) {
                    log.activity("Compress", dest)
                }
                Cmd.sh("gzip --best " + dest)
                dest = destZip
            }
            if (App.uid == 0 && dest.extension == "so" && Config.OS == "LINUX" && options.task == "Install") {
                if (verbose) {
                    log.activity("Ldconfig", dest)
                }
                Cmd.sh("ldconfig " + dest)
            }
            Path("install.log").append(dest + "\n")
        }
    }
    if (options.task != "Remove" && copied == 0) {
        log.error("No files copied for " + src)
    }
}


/*
    Read out/inc/buildConfig.h and set in options.build
 */
public function readBuildConfig(options)
{
    options.build = {}
    lines = options.top.join("out/inc/buildConfig.h").readLines()
    for (l in lines) {
        let line = lines[l]
        if (line.startsWith("#") || line.trim() == "" || line.startsWith("if")) {
            continue
        }
        if (line.contains("(BUILDING_NATIVE)")) {
            for (; l < lines.length; l++) {
                if (line.contains("Configuration for the Target")) {
                    break;
                }
            }
        }
        if (line.contains("EXPORT_OBJECTS")) {
            break
        }
        let [key, value] = line.trim().split("=")
        options.build[key] = value
    }
}


/*
    Prepare installation component prefixes
 */
public function preparePrefixes(options)
{
    let build = options.build
    build.BLD_TOP = options.top

    build.BUILD_BIN_DIR = Path(build.BLD_TOP).join("bin").portable
    build.BUILD_LIB_DIR = Path(build.BLD_TOP).join("lib").portable

    for each (prefix in ["BLD_PREFIX", "BLD_ROOT_PREFIX", "BLD_BIN_PREFIX", "BLD_CFG_PREFIX", "BLD_DOC_PREFIX", 
            "BLD_JEM_PREFIX", "BLD_INC_PREFIX", "BLD_LIB_PREFIX", "BLD_LOG_PREFIX", "BLD_MAN_PREFIX", "BLD_PRD_PREFIX", 
            "BLD_SAM_PREFIX", "BLD_SPL_PREFIX", "BLD_SRC_PREFIX", "BLD_VER_PREFIX", "BLD_WEB_PREFIX"]) {
        build["ORIG_" + prefix] = Path(build[prefix]).absolute.portable
        let tree: Path
        let root = Path(options.root)
        let path = Path(build[prefix])
        if (options.task == "Package") {
            tree = (prefix == "BLD_SRC_PREFIX") ? "/SRC" : "/BIN"
        } else {
            tree = ""
        }
        if (path.hasDrive) {
            path = "/" + path.components.slice(1).join("/")
        }
        build[prefix] = Path("" + root + tree + path).portable
    }
    let out = options.top.join("out")
    for (key in build) {
        let value = build[key].toString()
        if (value.contains("$")) {
            value = value.replace(/\$\{BLD_TOP}/g, options.top)
            value = value.replace(/\$\{BLD_OUT_DIR}/g, out)
            build[key] = value
        }
        if (key.contains("DIR")) {
            build[key] = Path(build[key]).portable
        }
    }
    build.ABS_BLD_TOP = Path(build.BLD_TOP).absolute.portable
    build.ABS_BLD_OUT_DIR = Path(build.BLD_OUT_DIR).absolute.portable
    build.ABS_BLD_BIN_DIR = Path(build.BLD_BIN_DIR).absolute.portable
    build.ABS_BLD_TOOLS_DIR = Path(build.BLD_TOOLS_DIR).absolute.portable
    if (build.BLD_VS) {
        build.BLD_VS = Path(build.BLD_VS).absolute.portable
    }
}

/*
    Fold long lines. On windows, will also convert line terminatations to <CR><LF>
 */
public function fold(path: Path, options)
{
    let lines = path.readLines()
    let out = new TextStream(new File(path, "wt"))
    for (l = 0; l < lines.length; l++) {
        let line = lines[l]
        if (options.fold && line.length > 80) {
            for (i = 79; i >= 0; i--) {
                if (line[i] == ' ') {
                    lines[l] = line.slice(0, i)
                    lines.insert(l + 1, line.slice(i + 1))
                    break
                }
            }
            if (i == 0) {
                lines[l] = line.slice(0, 80)
                lines.insert(l + 1, line.slice(80))
            }
        }
        out.writeLine(lines[l])
    }
    out.close()
}


/*
 */
public function expand(path: Path, options)
{
    let data = path.readString()
    for (token in options.build) {
        data = data.replace(RegExp("!!" + token + "!!", "g"), options.build[token])
    }
    path.write(data)
}


public function copySetup(options = null) 
{
    options ||= {}
    options.root ||= Path("/")
    options.task ||= "Install"
    options.top ||=  App.dir.findAbove("configure").dirname
    options.trace ||= App.getenv("TRACE")
    masterOptions = options

    options.root.makeDir()
    readBuildConfig(options)
    preparePrefixes(options)
    return options
}

