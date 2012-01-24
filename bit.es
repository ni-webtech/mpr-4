#!/usr/bin/env ejs
/*
    bit.es -- Build It! -- Embedthis Build It Framework
 */

module embedthis.bit {

class Bit 
{
    private const RC: String = '.bitrc'
    private const VERSION: Number = 0.1

    //  MOB - organize
    private var appName: String = 'bit'
    private var args: Args
    private var currentBitFile: Path
    private var currentComponent: String
    private var currentPlatform: String
    private var local: String
    private var options: Object
    private var out: Stream
    private var platforms: Array
    private var settings: Object
    private var rest: Array
    private var src: Path

    private var bareSpec: Object = { components: {}, targets: {} }
    private var spec: Object

    private var topTargets: Array

    private var posix = ['MACOSX', 'LINUX', 'UNIX', 'FREEBSD', 'SOLARIS']
    private var windows = ['WIN', 'WINCE']
    private var start: Date

    private var argTemplate = {
        options: {
            benchmark: { alias: 'b' },
            bit: { range: String },
            build: { range: String, separator: Array },
            config: { range: String, value: 'debug' },
            'continue': {},
            debug: {},
            diagnose: { alias: 'd' },
            disable: { range: String, separator: Array },
            enable: { range: String, separator: Array },
            init: { alias: 'i', range: String },
            log: { alias: 'l', range: String }
            out: { range: String }
            overwrite: { alias: 'o' },
            platform: { range: String, separator: Array },
            prefix: { range: String, separator: Array },
            release: {},
            save: { range: Path },
            show: { alias: 's'},
            verbose: { alias: 'v' },
            version: { alias: 'V' },
            why: {},
            'with': { range: String, separator: Array },
            without: { range: String, separator: Array },
        },
        usage: usage
    }

    function usage(): Void {
        print('\nUsage: bit [options] [targets|actions] ...\n' +
            '  Options:\n' + 
            '    --benchmark                # Measure elapsed time\n' +
            '    --build file.bit           # Build the specified bit file\n' +
            '    --config configuration     # Use the build configuration\n' +
            '    --continue                 # Continue on errors\n' +
            '    --diagnose                 # Emit diagnostic trace \n' +
            '    --debug                    # Same as --config debug\n' +
            '    --enable [feature=value]   # Enable a feature\n' +
            '    --disable feature          # Disable a feature\n' +
            '    --init path-to-source      # Initialize for building\n' +
            '    --log logSpec              # Save errors to a log file\n' +
            '    --out path                 # Save output to a file\n' +
            '    --overwrite                # Overwrite files\n' +
            '    --platform os-arch         # Build for this platforms\n' +
            '    --save path                # Save blended bit file\n' +
            '    --show                     # Show commands executed\n' +
            '    --release                  # Same as --config release\n' +
            '    --version                  # Dispay the bit version\n' +
            '    --verbose                  # Trace operations\n' +
            '    --with component[=PATH]    # Build with component at PATH\n' +
            '    --without component        # Build without a component\n' +
            '')
        App.exit(1)
    }

    function main() {
        let start = new Date
        global._b = this
        args = Args(argTemplate)
        options = args.options
        try {
            setup(args)
            if (options.init) {
                initialize()
            } else {
                process(options.build || findBitFile('local'))
            }
        } catch (e) {
            let msg: String
            if (e is String) {
                App.log.error('bit: Error: ' + e + '\n')
            } else {
                App.log.error('bit: Error: ' + ((options.diagnose) ? e : e.message) + '\n')
            }
            App.exit(2)
        }
        if (options.benchmark) {
            activity('Benchmark', "Elapsed time %.2f" % ((start.elapsed / 1000)) + " secs.")
        }
    }

    function setup(args: Args) {
        if (options.debug) {
            options.config = 'debug'
        }
        if (options.release) {
            options.config = 'release'
        }
        if (options.version) {
            print(version)
            App.exit(0)
        }
        if (options.log) {
            App.log.redirect(options.log)
            App.mprLog.redirect(options.log)
        }
        out = (options.out) ? File(options.out, "w") : stdout
        local =  Config.OS.toLower() + '-' + Config.CPU + '-' + options.config
        platforms = options.platform || []
        if (platforms[0] != local) {
            platforms.insert(0, local)
        }
        bin =  Path('out').join(local + '-' + options.config, 'bin')
        App.putenv('PATH', bin + App.SearchSeparator + App.getenv('PATH'))
        App.log.debug(2, "PATH=" + App.getenv('PATH'))
    }

    function initialize() {
        src = Path(options.init)
        if (!src.join('bit/standard.bit').exists) {
            throw 'Can\'t find bit configuration at "' + src + '"'
        }
        for each (platform in platforms) {
            currentPlatform = platform
            let [os, arch] = platform.split('-') 
            global.spec = spec = bareSpec.clone()
            /* Read to get settings */
            loadWrapper(src.join('bit/standard.bit'))
            loadWrapper(src.join('bit/os/' + os + '.bit'))
            loadWrapper(src.join('product.bit'))
            trace('Init', spec.settings.title)

            setCommandLineOptions()
            setConfiguration()
            setDirectories()
            expandTokens(spec)
            setTypes()
            findComponents()
            makeOutDirs()
            makeBitFile(platform)
        }
    }

    function makeBitFile(platform) {
        let nspec = {}
        if (platforms.length > 1 && platform == platforms[0]) {
            nspec.cross = platforms.slice(1)
        }
        let [os, arch] = platform.split('-') 
        blend(nspec, {
            blend : [
                'bit/standard.bit',
                'bit/os/' + os + '.bit',
                src.join('product.bit'),
            ],
            platform: {
                os: os,
                arch: arch,
            },
            directories: { 
                src: src.absolute,
            },
            settings: spec.settings,
            components: spec.components,
        })
        if (spec.env) {
            nspec.env = spec.env
        }
        if (platform == local) {
            platform = 'local'
        }
        let path: Path = Path(platform).joinExt('bit')
        if (path.exists && !options.overwrite) {
            throw 'The ' + path + ' file already exists. Use bit --overwrite'
        }
        trace('Generate', path)
        path.write('/*\n    ' + platform + '.bit -- Build ' + spec.settings.title + ' for ' + platform + 
            '\n\n    Generated by bit.\n */\n\nbit(' + 
            serialize(nspec, {pretty: true, indent: 4, commas: true, quotes: false}) + ')\n')
        if (options.show) {
            trace('Configuration', spec.settings.title + ' for ' + spec.OS + ', ' + spec.ARCH + 
                '\nsettings = ' +
                serialize(nspec.settings, {pretty: true, indent: 4, commas: true, quotes: false}) +
                '\nComponents = ' +
                serialize(nspec.components, {pretty: true, indent: 4, commas: true, quotes: false}))
        }
    }

    function setCommandLineOptions() {
        for each (field in options.disable) {
            spec.settings[field] = false
        }
        for each (field in options.enable) {
            let [field,value] = value.split('=')
            spec.settings[field] = value || true
        }
        for each (value in options['with']) {
            let [field,value] = value.split('=')
            spec.components[field] = { path: Path(value) }
        }
        for each (field in options['without']) {
            spec.components[field] = { disable: true, diagnostic: 'initialized --without' }
        }
        for each (field in options['prefix']) {
            let [field,value] = value.split('=')
            spec.prefixes[field] = Path(value)
        }
    }

    function setConfiguration() {
        spec.settings.configuration = options.config
        blend(spec, spec.configurations[options.config], {combine: true})
    }

    function setDirectories() {
        if (src) {
            spec.directories.src = src
        } else {
            // For a relative path
            spec.directories.src = Path(spec.directories.src).relativeFrom('.')
            src = spec.directories.src
        }
        setTypes()
    }

    function findComponents() {
        trace('Find', 'Components')
        for each (component in spec.required + spec.optional) {
            if (spec.components[component] && spec.components[component].disable) {
                continue
            }
            let path = src.join('bit/components', component + '.bit')
            vtrace('Find', 'Component ' + component)
            if (path.exists) {
                try {
                    spec.components[component] ||= {}
                    currentComponent = component
                    loadWrapper(path)
                } catch (e) {
                    if (!(e is String)) {
                        App.log.debug(0, e)
                    }
                    let kind = spec.required.contains(component) ? 'Required' : 'Optional'
                    whyMissing(kind + ' componenent "' + component + '" ' + e)
                    spec.components[component] = { diagnostic: "" + e }
                }
            } else {
                throw "Unknown component " + path
            }
            if (options.verbose) {
                activity('Probe', 'Found ' + component + ' at ' + spec.components[component].path)
            }
        }
    }

    public function probe(file: Path, control = {}): Path {
        let path: Path
        if (!file.exists) {
            let search = []
            let dir
            if (dir = spec.components[currentComponent].path) {
                search.push(dir)
            }
            if (control.search && control.search is Array) {
                search += control.search
            }
            App.log.debug(2, "Probe for " + file + ' in ' + search)
            for each (let s: Path in search) {
                App.log.debug(2, "Probe for " + s.join(file) + ' exists: ' + s.join(file).exists)
                if (s.join(file).exists) {
                    path = s.join(file)
                    break
                }
            }
            path ||= Cmd.locate(file)
        }
        if (!path) {
            throw 'component ' + file + ' not found'
        }
        App.log.debug(2, 'Probe for ' + file + ' found at ' + path)
        if (control.fullpath) {
            return path
        }
        return path.toString().replace(RegExp('[/\\\\]' + file + '$'), '')
    }

    function process(path: Path) {
        global.spec = spec = bareSpec.clone()
        loadWrapper(path)

        let startPlatform = spec.platform.os + '-' + spec.platform.arch
        currentPlatform = startPlatform

        setConfiguration()
        setDirectories()
        expandTokens(spec)
        makeOutDirs()

        build()

        for each (platform in spec.cross) {
            if (platform == startPlatform) continue
            currentPlatform = platform
            process(Path(platform).joinExt('bit'))
        }
    }

    function loadWrapper(path) {
        let saveCurrent = currentBitFile
        currentBitFile = path
        vtrace('Loading', currentBitFile)
        load(path)
        currentBitFile = saveCurrent
    }

    /*
        Change paths in a bit file to be relative to the bit file
     */
    function loadRebase(o) {
        let base = currentBitFile.dirname
        if (o.common) {
            let cinc = o.common.includes
            for (i in cinc) {
                cinc[i] = base.join(cinc[i])
            }
            cinc = o.common['+includes']
            for (i in cinc) {
                cinc[i] = base.join(cinc[i])
            }
        }
        for (tname in o.targets) {
            o.targets[tname].name ||= tname
        }
        for each (target in o.targets) {
            if (target.includes is Array) {
                for (i in target.includes) {
                    target.includes[i] = base.join(target.includes[i])
                }
            } else if (target.includes is RegExp) {
                ;
            } else if (target.includes) {
                target.includes = base.join(target.includes)
            }
            for (i in target.sources) {
                if (target.sources is Array) {
                    target.sources[i] = base.join(target.sources[i])
                } else if (target.includes is Regexp) {
                    ;
                } else if (target.sources) {
                    target.sources = base.join(target.sources)
                }
            }
        }
    }

    public function loadBitfile(o) {
        let base = currentBitFile.dirname
        loadRebase(o)
        let toBlend = blend({}, o, {combine: true})
        /* Blending is depth-first. So blend existing spec over blended spec */
        /* Load blended bit files first */
        for each (path in toBlend.blend) {
            loadWrapper(base.join(path))
        }
        global.spec = blend(spec, o, {combine: true})
    }

    function findBitFile(name: Path) {
        name = name.joinExt('bit')
        let base: Path = currentBitFile || '.'
        for (let d: Path = base; d.parent != d; d = d.parent) {
            let f: Path = d.join(name)
            if (f.exists) {
                return f
            }
        }
        throw 'Can\'t locate ' + name
    }

    function prepBuild() {
        setTokens()
        selectTargets()
        setTypes()
        blendCommon()
        resolveDependencies()
        expandWildcards()
        setTypes()
        setTargetPaths()
        Object.sortProperties(spec);
        //MOB - reenable spec.settings = Object.sortProperties(spec.settings);

        if (options.save) {
            delete spec.blend
            options.save.write(serialize(spec, {pretty: true, commas: true, indent: 4, quotes: false}))
            trace('Save', "Combined Bit files to: " + options.save)
            App.exit()
        }
        trace('Building', currentPlatform)
        trace('Targets', topTargets)
    }

    function selectTargets() {
        topTargets = args.rest
        if (topTargets.length == 0) {
            for (let [tname,target] in spec.targets) {
                if (target.type && target.type != 'action') {
                    topTargets.push(tname)
                }
            }
        } else {
            /* Implement the "build" target */
            let index = topTargets.indexOf("build")
            if (index >= 0) {
                let names = []
                for (let [tname,target] in spec.targets) {
                    if (target.type && target.type != 'action') {
                        names.push(tname)
                    }
                }
                topTargets.splice(index, 1, ...names)
            }
        }
        topTargets = topTargets.sort()
        for (let [index, tname] in topTargets) {
            let target = spec.targets[tname]
            if (!target) {
                throw "Unknown target " + tname
            }
        }
        for (let [tname, target] in spec.targets) {
            if (target.enable) {
                let script = target.enable.expand(spec, {fill: ''})
                if (!eval(script)) {
                    vtrace('Skip', 'Target ' + tname + ' is disabled on this platform') 
                    target.skip = true
                }
                target.name ||= tname
            }
            if (target.platforms) {
                if (!target.platforms.contains(currentPlatform)) {
                    if (local && !target.platforms.contains('local')) {
                        if (!target.platforms.contains('local')) {
                            target.skip = true
                        }
                    }
                }
            }
        }
        vtrace('Targets', topTargets)
    }

    function setTargetPaths() {
        for each (target in spec.targets) {
            if (!target.path) {
                if (target.type == 'lib') {
                    target.path = spec.directories.lib.join(target.name).joinExt(spec.extensions.shobj)
                } else if (target.type == 'obj') {
                    target.path = spec.directories.obj.join(target.name).joinExt(spec.extensions.obj)
                } else if (target.type == 'exe') {
                    target.path = spec.directories.bin.join(target.name).joinExt(spec.extensions.exe)
                } else {
                    target.path = Path(target.name)
                }
            }
        }
    }

    function buildFileList(include, exclude)
    {
        let files
        if (include is RegExp) {
            //  MOB - should be relative to the bit file that created this
            files = Path(src).glob('*', {include: include})
        } else if (include is Array) {
            files = []
            for each (pattern in include) {
                files += Path('.').glob(pattern)
            }
        } else {
            files = Path('.').glob(include)
        }
        if (exclude) {
            if (exclude is RegExp) {
                files = files.reject(function (elt) elt.match(exclude)) 
            } else if (exclude is Array) {
                for each (pattern in exclude) {
                    files = files.reject(function (elt) { return elt.match(pattern); } ) 
                }
            } else {
                files = files.reject(function (elt) elt.match(exclude))
            }
        }
        return files
    }

    function resolveDependencies() {
        let index
        for each (target in spec.targets) {
            for each (dname in target.depend) {
                let dep = spec.targets[dname]
                if (dep) {
                    if (dep.type == 'lib') {
                        target.libraries
                        target.libraries ||= []
                        target.libraries.push(dname.replace(/^lib/, ''))
                    }
                } else {
                    let component = spec.components[dname]
                    if (component) {
                        if (component.includes) {
                            target.includes ||= []
                            target.includes += component.includes
                        }
                        if (component.libraries) {
                            target.libraries ||= []
                            target.libraries += component.libraries
                        }
                        if (component.linker) {
                            target.linker ||= []
                            target.linker += component.linker
                        }
                        /*
                        //  MOB KEEP - this may be required on windows
                        if (component.libraries) {
                            target.libraries += component.libraries
                        }
                        */
                    }
                }
            }
        }
    }

    /*
        Expand target.sources and target.headers. Support include+exclude and create target.files[]
     */
    function expandWildcards() {
        let index
        for each (target in spec.targets) {
            if (target.headers) {
//  MOB - what does this actually do?
                let files = buildFileList(target.headers.include, target.headers.exclude)
                for each (file in files) {
                    //  Create a target for each header
                    let header = spec.directories.inc.join(file.basename)
                    let newTarget = { name : file, path: header, type: 'header', files: [ file ] }
                    if (spec.targets[file]) {
                        newTarget = blend(spec.targets[file], newTarget, {combined: true})
                    } else {
                        spec.targets[newTarget.name] = newTarget
                    }
                    spec.targets.push(newTarget)
                }
            }
            if (target.sources) {
                target.files = []
                let files = buildFileList(target.sources, target.exclude)
                for each (file in files) {
                    /*
                        Create a target for each source file
                     */
                    let obj = spec.directories.obj.join(file.replaceExt(spec.extensions.obj).basename)
            /*
                    let includes = spec.common.includes
                    if (target.includes) {
                        includes = includes + target.includes
                    }
                    */
                    let newTarget = { name : obj, path: obj, type: 'obj', files: [ file ], includes: target.includes }
                    if (spec.targets[obj]) {
                        newTarget = blend(spec.targets[newTarget.name], newTarget, {combined: true})
                    } else {
                        spec.targets[newTarget.name] = newTarget
                    }
                    /*
                        This is an implicit dependency by position in targets
                     */
                    newTarget.depend = depend(newTarget)
                    target.files.push(obj)
                    target.depend ||= []
                    target.depend.push(obj)
                }
            }
        }
    }

    function blendCommon() {
        if (spec.common.preblend) {
            let preblend = spec.common.preblend.expand(spec, {fill: '${}'})
            eval(preblend)
        }
        let common = {}
        for (name in spec.common) {
            common['+' + name] = spec.common[name]
        }
        for each (target in spec.targets) {
            if (target.type && (target.type == 'obj' || target.type == 'lib' || target.type == 'exe')) {
                if (target.postblend) {
                    let postblend = target.postblend.expand(spec, {fill: '${}'})
                    eval(postblend)
                }
                blend(target, common, {combine: true})
                //  MOB - cleanup somehow
                if (target.preblend) {
                    delete target.preblend
                }
                if (target.type == 'obj') { 
                    delete target.linker 
                    delete target.libraries 
                }
            }
        }
    }

    function setTypes() {
        for (let [key,value] in spec.directories) {
            spec.directories[key] = Path(value).natural
        }
        for each (target in spec.targets) {
            if (target.path) {
                target.path = target.path cast Path
            }
        }
        for (let [pname, prefix] in spec.prefixes) {
            spec.prefixes[pname] = prefix cast Path
        }
    }

    function build() {
        prepBuild()
        for each (tname in topTargets) {
            let target = spec.targets[tname]
            buildTarget(target)
        }
        trace('Complete', '')
    }

    function buildTarget(target) {
        for each (dname in target.depend) {
            let dep = spec.targets[dname]
            if (!dep) {
                if (!Path(dname).exists) {
                    if (!spec.components[dname]) {
                        print('Unknown dependency "' + dname + '" in target "' + target.name + '"')
                        return
                    }
                }
            } else {
                if (dep.skip || dep.built) {
                    continue
                }
                buildTarget(dep)
            }
        }
        if (target.message) {
            trace('Info', target.message)
        }
        if (target.script) {
            let script = target.script.expand(spec, {fill: '${}'})
            eval(script)
        }
        if (target.type == 'action') {
            buildAction(target)
        } else if (target.type == 'lib') {
            buildLib(target)
        } else if (target.type == 'exe') {
            buildExe(target)
        } else if (target.type == 'obj') {
            buildObj(target)
        } else {
            dump(target)
            throw 'Unknown target type in ' + target.path
        }
        target.built = true
    }

    function buildExe(target) {
        if (!stale(target)) {
            whySkip(target.path, 'is up to date')
            return
        }
        diagnose('Building:\n' + target.path + ' = ' + serialize(target, {pretty: true}))
        if (options.diagnose) {
            dump('TARGET', target)
        }
        let transition = target.rule || 'exe'
        let rule = spec.rules[transition]
        if (!rule) {
            throw 'No rule to build target ' + target.path + ' for transition ' + transition
            return
        }
        spec.target = target
        spec.PREPROCESS = ''
        spec.OUT = target.path
        spec.IN = target.files.join(' ')
        spec.LIBS = mapLibs(target.libraries)

        /* Double expand so rules tokens can use ${OUT} */
        let command = rule.expand(spec, {fill: ''})
        command = command.expand(spec, {fill: ''})
        trace('Link', target.name)
        diagnose(2, command)
        let cmd = runCmd(command)
        if (cmd.status != 0) {
            throw 'Build failure for ' + target.path + '\n' + cmd.error + "\n" + cmd.response
        }
    }

    function buildLib(target) {
        //  MOB - need libraries[] so we can stat them
        if (!stale(target)) {
            whySkip(target.path, 'is up to date')
            return
        }
        diagnose('Building:\n' + serialize(target, {pretty: true}))
        if (options.diagnose) {
            dump('TARGET', target)
        }
        buildSym(target)
        let transition = target.rule || 'lib'
        let rule = spec.rules[transition]
        if (!rule) {
            throw 'No rule to build target ' + target.path + ' for transition ' + transition
            return
        }
        spec.target = target
        spec.OUT = target.path
        spec.IN = target.files.join(' ')
        spec.DEF = Path(target.path.toString().replace(/dll$/, 'def'))
        spec.LIBS = mapLibs(target.libraries)

        /* Double expand so rules tokens can use ${OUT} */
        let command = rule.expand(spec, {fill: ''})
        command = command.expand(spec, {fill: ''})
        trace('Link', target.name)
        let cmd = runCmd(command)
        if (cmd.status != 0) {
            throw 'Build failure for ' + target.path + '\n' + cmd.error + "\n" + cmd.response
        }
    }

    /*
        Build symbols file for windows libraries
        MOB - should be selectable
     */
    function buildSym(target) {
        let rule = spec.rules['sym']
        if (!rule) {
            return
        }
        spec.IN = target.files.join(' ')
/*
UNUSED
        spec.target = target
        spec.OUT = target.path
        spec.LIBS = mapLibs(target.libraries)
*/

        /* Double expand so rules tokens can use ${OUT} */
        let command = rule.expand(spec, {fill: ''})
        command = command.expand(spec, {fill: ''})
        trace('Symbols', target.name)
        let cmd = runCmd(command)
        if (cmd.status != 0) {
            throw 'Build failure for ' + target.path + '\n' + cmd.error + "\n" + cmd.response
        }
        let data = cmd.response
        let result = []
        let lines = data.match(/SECT.*External *\| .*/gm)
        for each (l in lines) {
            if (l.contains('__real')) continue
            let sym = l.replace(/.*\| _/, '').replace(/\r$/,'')
            result.push(sym)
        }
        let def = Path(target.path.toString().replace(/dll$/, 'def'))
        def.write('LIBRARY ' + target.name + '.dll\nEXPORTS\n  ' + result.sort().join('\n  '))
    }

    function buildObj(target) {
        if (!stale(target)) {
            return
        }
        diagnose('Building:\n' + serialize(target, {pretty: true}))
        if (options.diagnose) {
            dump('TARGET', target)
        }
        let ext = target.path.extension
        for each (file in target.files) {
            let transition = file.extension + '->' + target.path.extension
            let rule = target.rule || spec.rules[transition]
            if (!rule) {
                rule = spec.rules[target.path.extension]
                if (!rule) {
                    throw 'No rule to build target ' + target.path + ' for transition ' + transition
                    return
                }
            }
            //  MOB - rethink how this is done
            spec.target = target
            spec.PREPROCESS = ''
            spec.OUT = target.path
            spec.IN = file
            spec.CFLAGS = (target.compiler) ? target.compiler.join(' ') : ''
//MOB - fixup CFLAGS
            spec.INCLUDES = (target.includes) ? target.includes.map(function(e) '-I' + e) : ''

            let command = rule.expand(spec, {fill: ''})
            trace('Compile', file)
            let cmd = runCmd(command)
            if (cmd.status != 0) {
                throw 'Build failure for ' + target.path + '\n' + cmd.error + "\n" + cmd.response
            }
        }
    }

    function buildAction(target) {
        if (!stale(target)) {
            return
        }
        diagnose('Building:\n' + serialize(target, {pretty: true}))
        if (options.diagnose) {
            dump('TARGET', target)
        }
        spec.target = target
        let script = target.script.expand(spec, {fill: ''})
        eval(script)
    }

    function mapLibs(libs: Array): Array {
        if (spec.OS == 'WIN') {
            libs = libs.clone()
            for (i in libs) {
                let lib = libs[i]
                lib = spec.directories.lib.join("lib" + lib).joinExt(spec.extensions.shlib)
                if (lib.exists) {
                    libs[i] = lib
                }
            }
        } else {
            libs = libs.map(function(e) '-l' + Path(e).trimExt().toString().replace(/^lib/, '') )
        }
        return libs
    }

    /*
        Test if a target is stale vs the inputs AND dependencies
     */
    function stale(target) {
        let path = target.path
        if (!path.modified) {
            whyRebuild(path, 'Rebuild', 'is missing.')
            return true
        }
        for each (file in target.files) {
            if (file.modified > path.modified) {
                whyRebuild(path, 'Rebuild', 'input ' + file + ' has been modified.')
                return true
            }
        }
        for each (let dep: Path in target.depend) {
            if (!spec.targets[dep]) {
                /* If dependency is not a target, then treat as a file */
                if (!dep.modified) {
                    whyRebuild(path, 'Rebuild', 'missing dependency ' + dep)
                    return true
                }
                if (dep.modified > path.modified) {
                    whyRebuild(path, 'Rebuild', 'dependency ' + dep + ' has been modified.')
                    return true
                }
                return false
            }
            let file = spec.targets[dep].path
            if (file.modified > path.modified) {
                whyRebuild(path, 'Rebuild', 'dependent ' + file + ' has been modified.')
                return true
            }
        }
        diagnose('Building:\n' + serialize(target, {pretty: true}))
        return false
    }

    function depend(target): Array {
        let includes: Array = []
        for each (path in target.files) {
            let str = path.readString()
            //  MOB - remove when array += null is a NOP
            let more = str.match(/^#include.*"/gm)
            if (more) {
                includes += more
            }
        }
        let depends = []
        for each (item in includes) {
            let ifile = item.replace(/#include.*"(.*)"/, '$1')
            let path
            for each (dir in target.includes) {
                path = Path(dir).join(ifile)
                if (path.exists && !path.isDir) {
                    break
                }
                path = null
            }
            if (path) {
                depends.push(path)
            } else {
                App.log.error('Can\'t find include file ' + ifile + ' for ' + target.name)
            }
        }
        return depends
    }

    function setTokens() {
        spec.ARCH = spec.platform.arch
        spec.OS = spec.platform.os.toUpper()
    }

    function expandTokens(o) {
        for (let [key,value] in o) {
            if (value is String) {
                o[key] = value.expand(spec, {fill: '${}'})
            } else if (value is Path) {
                o[key] = Path(value.toString().expand(spec, {fill: '${}'}))
            } else if (Object.getOwnPropertyCount(value) > 0) {
                o[key] = expandTokens(value)
            }
        }
        return o
    }

    public function runCmd(command: String, coptions = null): Cmd {
        if (options.show) {
            activity('Run', command)
        }
        let cmd = new Cmd
        if (spec.env) {
            let env = {}
            for (let [key,values] in spec.env) {
                env[key] = values.join(App.SearchSeparator)
            }
            cmd.env = env
        }
        cmd.start(command, coptions)
        if (options.show && cmd.status == 0) {
            out.write(cmd.response + ' ' + cmd.error)
        }
        return cmd
    }

    function makeOutDirs() {
        for each (d in spec.directories) {
            Path(d).makeDir()
        }
    }

    public function activity(tag: String, ...args): Void {
        let msg = args.join(" ")
        let msg = "%12s %s" % (["[" + tag + "]"] + [msg]) + "\n"
        out.write(msg)
    }

    function trace(tag, msg) {
        activity(tag, msg)
    }

    function vtrace(tag, msg) {
        if (options.verbose) {
            activity(tag, msg)
        }
    }

    function whyRebuild(path, tag, msg) {
        if (options.why) {
            activity(tag, path + ' because ' + msg)
        }
    }

    function whySkip(path, msg) {
        if (options.why) {
            activity('Target', path + ' ' + msg)
        }
    }

    function whyMissing(msg) {
        if (options.why) {
            activity('Init', msg)
        }
    }

    function diagnose(msg) {
        if (options.diagnose) {
            activity('Debug', msg)
        }
    }

    public function action(cmd: String) {
        if (cmd == 'clean') {
            for each (target in spec.targets) {
                target.path.remove()
                vtrace('Clean', target.path)
            }
        } else if (cmd == 'compile') {
        }
    }
}

} /* bit module */


require embedthis.bit

public var b: Bit = new Bit
b.main()

public function bit(o: Object) {
    b.loadBitfile(o)
}

public function probe(file: Path, options = {}): Path {
    return b.probe(file, options)
}

public function program(name)
{
    let components = {}
    components[name] = { path: probe(name, {fullpath: true})}
    bit({components: components})
}

public function activity(tag, msg)
    b.activity(tag, msg)

public function run(command, options = null)
    b.runCmd(command, options)

/*
    @copy   default
  
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
  
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.TXT distributed with
    this software for full details.
  
    This software is open source; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version. See the GNU General Public License for more
    details at: http://www.embedthis.com/downloads/gplLicense.html
  
    This program is distributed WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
    This GPL license does NOT permit incorporating this software into
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses
    for this software and support services are available from Embedthis
    Software at http://www.embedthis.com
  
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
