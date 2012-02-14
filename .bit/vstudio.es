/*
   vstudio.es -- Support functions for generating VS projects
        
   Exporting: vstudio()

   Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
*/     
    
require ejs.unix
    
var out: Stream

const TOOLS_VERSION = '4.0'
const PROJECT_FILE_VERSION = 10.0.30319.1

public function vstudio(base: Path) {
    bit.TOOLS_VERSION = TOOLS_VERSION
    bit.PROJECT_FILE_VERSION = PROJECT_FILE_VERSION
    for each (target in bit.targets) {
        vsbuild(base, target)
    }
}

function vsbuild(base: Path, target) {
    if (target.built || !target.enable) {
        return
    }
    if (target.type != 'exe' && target.type != 'lib') {
        return
    }
    for each (dname in target.depends) {
        let dep = bit.targets[dname]
        if (dep.enable && !dep.built) {
            vsbuild(base, dep)
        }
    }
    let path = base.join(target.name).joinExt('vcxproj').relative
    trace('Generate', path)
    //  MOB - this must put the file into text mode and add '\r' to line endings
    out = TextStream(File(path, 'wt'))
    vsheader(target)
    vsconfiguration(target)
    vslinkoptions(target)
    vsresources(target)
    vsdependencies(target)
    vstrailer(target)
    out.close()
}

function vsheader() {
    output('<?xml version="1.0" encoding="utf-8"?>')
    output('<Project DefaultTargets="Build" ToolsVersion="${TOOLS_VERSION}" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">')
}

function vsconfiguration(target) {
    let subsystem = (target.rule == 'gui') ? 'Windows' : 'Console'
    bit.PTYPE = (target.type == 'exe') ? 'Application' : 'DynamicLibrary'
    bit.VTYPE = 'Win32'
    bit.GUID = target.guid = Cmd('uuidgen').response.toLower().trim()
    bit.CTOK = '$(Configuration)'
    bit.PTOK = '$(Platform)'
    bit.STOK = '$(SolutionDir)'
    bit.OTOK = '$(OutDir)'
    bit.UTOK = '$(UserRootDir)'
    bit.VTOK = '$(VCTargetsPath)'
    bit.NAME = target.name
    bit.OUTDIR = wpath(bit.dir.obj.join(target.name))

    output('
<Import Project="product.props" />
<ItemGroup Label="ProjectConfigurations">
  <ProjectConfiguration Include="Debug|${VTYPE}">
    <Configuration>Debug</Configuration>
    <Platform>${VTYPE}</Platform>
  </ProjectConfiguration>
</ItemGroup>

<PropertyGroup Label="Globals">
  <ProjectGuid>{${GUID}}</ProjectGuid>
  <RootNamespace />
  <Keyword>${VTYPE}Proj</Keyword>
</PropertyGroup>

<Import Project="${VTOK}\Microsoft.Cpp.Default.props" />

<PropertyGroup Condition="\'${CTOK}|${PTOK}\'==\'Debug|${VTYPE}\'" Label="Configuration">
  <ConfigurationType>${PTYPE}</ConfigurationType>
  <CharacterSet>NotSet</CharacterSet>
</PropertyGroup>

<Import Project="${VTOK}\Microsoft.Cpp.props" />

<ImportGroup Label="ExtensionSettings">
</ImportGroup>

<PropertyGroup Label="UserMacros" />

<PropertyGroup>
  <_ProjectFileVersion>${PROJECT_FILE_VERSION}</_ProjectFileVersion>
  <OutDir Condition="\'${CTOK}|${PTOK}\'==\'Debug|${VTYPE}\'">${OUTDIR}\\bin\\</OutDir>
  <IntDir Condition="\'${CTOK}|${PTOK}\'==\'Debug|${VTYPE}\'">${OUTDIR}\\obj\\${NAME}\\</IntDir>
</PropertyGroup>
    ')
}

//  MOB - should emit headers for all source that depends on headers
function vsheaders(target) {
    /*
    if (target.type == 'header') {
    output('<ItemGroup>')
        output('  <ClInclude Include="' + wpath(target.path) + '" />')
    }
    output('<ItemGroup>')
    */
}

function vssources(target) {
    output('<ItemGroup>')
    for each (file in target.files) {
        output('  <ClCompile Include="' + wpath(file) + '" />')
    }
    output('<ItemGroup>')
}

//  MOB - TODO
function vsresources(target) {
    output('<ItemGroup>')
    for each (file in target.files) {
        output('  <ClCompile Include="' + wpath(file) + '" />')
    }
    output('<ItemGroup>')
}

function vslinkoptions(target) {
    let def = Path(target.path.toString().replace(/dll$/, 'def'))
    if (def.exists) {
        bit.DEF = def
        bit.LIBS = target.libraries.join(';')
        output('
<ItemDefinitionGroup>
<Link>
  <ModuleDefinitionFile>${DEF}</ModuleDefinitionFile>
</Link>
</ItemDefinitionGroup>
<ItemDefinitionGroup>
<Link>
  <AdditionalDependencies>$LIBS;%(AdditionalDependencies)</AdditionalDependencies>
</Link>
</ItemDefinitionGroup>
<ItemDefinitionGroup>
<Link>                                                                                                 
  <SubSystem>Windows</SubSystem>                                                                       
</Link>                                                                                                
</ItemDefinitionGroup> 
        ')
    }
}

function vsdependencies(target) {
    for each (dname in target.depends) {
        let dep = bit.targets[dname]
        if (!dep) {
            throw 'Missing dependency ' + dname + ' for target ' + target.name
        }
        //  MOB - should do something with objects, headers
        if (dep.type != 'exe' && dep.type != 'lib') {
            continue
        }
        if (!dep.enable) {
            continue
        }
        if (!dep.guid) {
            throw 'Missing guid for ' + dname
        }
        bit.DEP = dname
        bit.GUID = dep.guid
        output('
<ItemGroup>
  <ProjectReference Include="${DEP}.vcxproj">
  <Project>${GUID}</Project>
  <ReferenceOutputAssembly>false</ReferenceOutputAssembly>
  </ProjectReference>
</ItemGroup>')
    }
}

function vstrailer() {
    output('\n<Import Project="${VTOK}\Microsoft.Cpp.targets" />')
    output('<ImportGroup Label="ExtensionTargets">\n</ImportGroup>\n\n</Project>')
}

function output(line: String) {
    out.writeLine(line.expand(bit))
}

function wpath(path: Path)
    Path(path.relative.toString().replace(/\//g, '\\'))
