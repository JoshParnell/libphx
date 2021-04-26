#!/usr/local/bin/luajit

local LIBPATH = arg[1]
local LIBNAME = arg[2]
local OUTPATH = arg[3]
local VERBOSE = arg[4]

local insert = table.insert
local remove = table.remove
local join   = table.concat

local function format (str, ...)
  local index, args = 0, { ... }
  local function replaceStar (match)
    index = index + 1
    return match:gsub('%*', function (star)
      return remove(args, index)
    end)
  end

  -- NOTE : %d*%*? should be %d*|%*? but Lua doesn't support |
  str = str:gsub('%%[-+ #0]?%d*%*?%.?%d*%*?', replaceStar)
  return string.format(str, unpack(args))
end

local function appendf(t, fmt, ...) insert(t, format(fmt, ...)) end
local function logf(fmt, ...) if VERBOSE then print('  '..format(fmt, ...)) end end
local function warnf(fmt, ...) print('  Warning: '..format(fmt, ...)) end
local function errorf(fmt, ...) error(format(fmt, ...)) end

function listdir (root, recurse)
  local root = root:gsub('/$','')
  local files = {}
  local recurse = recurse or true
  for file in lfs.dir(root) do
    if file ~= '.' and file ~= '..' then
      local path = root .. '/' .. file
      local mode = lfs.attributes(path, 'mode')
      if mode == 'file' then
        files[#files + 1] = path
      elseif recurse and mode == 'directory' then
        local result = listdir(path, recurse)
        for i = 1, #result do files[#files + 1] = result[i] end
      end
    end
  end
  table.sort(files)
  return files
end


local isenum = {}
local isopaque = {}
local isstruct = {}

local scopes = {}
local enums = {}
local opaques = {}
local structs = {}
local typedefs = {}
local funcPtrs = {}

local maxScope = 0
local maxOpaque = 0
local maxTypedef = 0
local maxFuncPtr = 0
local maxFuncPtrRet = 0

local function getScope (name)
  for _, scope in ipairs(scopes) do
    if scope.name == name then
      return scope
    end
  end

  insert(scopes, {
    name = name,
    elems = {},
    enums = {},
    maxName = 0,
    maxSymbol = 0,
    maxType = 0,
  })
  maxScope = math.max(maxScope, #name)
  return scopes[#scopes]
end

local function getEnumScope (name, enums)
  for _, enum in ipairs(enums) do
    if enum.name == name then
      return enum
     end
  end

  insert(enums, {
    name = name,
    keys = {},
    subscopes = {},
    maxKey = 0,
  })
  return enums[#enums]
end

local function getStruct(name)
  for _, struct in ipairs(structs) do
    if struct.name == name then
      return struct
    end
  end

  insert(structs, {
    name = name,
    fields = {},
    dependencies = {},
    maxFieldType = 0,
    nPreprocessor = 0,
  })
  return structs[#structs]
end

local function parseOpaque (symbol)
  if not isopaque[symbol] then
    isopaque[symbol] = true
    insert(opaques, symbol)
    maxOpaque = math.max(maxOpaque, #symbol)
  end
end

local function parseStructDef (name)
  if not isstruct[name] then
    isstruct[name] = true
    getStruct(name)
    getScope(name)
  end
end

local function parseStructField (struct, line)
  local pType, fPtr, fName, fArray = line:match('([%w_]+)([%s%*]*)([%w_]+) ?(%[? ?%d*% ?]?)')
  fArray = fArray:gsub('%s', '')
  fPtr = fPtr:gsub('%s', '')

  local fType = pType..fPtr
  insert(struct.fields, {
    name = fName..fArray,
    type = fType,
    pType = pType,
    isPtr = fPtr ~= '',
    isArray = fArray ~= '',
  })
end

local function parseStructMacro (struct, macro, args)
  if macro == 'ArrayList' then
    local fType, fName = args:match('(%g+) ?, ?(%g+)')
    parseStructField(struct, format('int32 %s_size;', fName))
    parseStructField(struct, format('int32 %s_capacity;', fName))
    parseStructField(struct, format('%s* %s_data;', fType, fName))

  else
    warnf('Unrecognized macro %s in transparent struct %s', macro, struct.name)
  end
end

local function parseStruct (name, body)
  --[[ NOTE : Struct parsing is super naive. Parsing is lexical, not syntactic.
              We don't handle anything funky like unions, nested structs, etc.
              Const pointers are not handled. Arrays of transparent structs are
              not expanded. ArrayList is special cased. ]]

  local struct = getStruct(name)

  -- Remove Preprocessor Directives
  body, n1 = body:gsub('#.-\n', '')
  body, n2 = body:gsub('#[^\n]*$', '')
  struct.nPreprocessor = n1 + n2

  -- Remove Extra Space
  body = body:gsub('%s+', ' ')

  for line in body:gmatch('.-;') do
    -- Expand Macros
    local macro, args = line:match('(%g+) ?%((.-)%) ?;')
    if macro then
      parseStructMacro(struct, macro, args)

    -- Expand Comma Separated Declarations
    elseif line:find(',') then
      local fType = line:match('^ ?([%w_]+)')
      local fields = line:gsub(',', format('; %s', fType))
      for subline in fields:gmatch('.-;') do
        parseStructField(struct, subline)
      end

    -- Regular Declaration
    else
      parseStructField(struct, line)
    end
  end
end

local function parseEnumDef (name)
  if not isenum[name] then
    isenum[name] = true
    local scope = getScope(name)
    local enumScope = getEnumScope(name, enums)
    insert(scope.enums, enumScope)
  end
end

local function parseEnum (type, symbol)
  local eName, eKey = symbol:match('^(%g-)_(%g+)$')
  if not eName or not eKey then return end
  local scope = getScope(eName)
  local enumScope = getEnumScope(eName, enums)
  insert(scope.elems, {
    name = eKey,
    kind = 'enum',
    type = type,
    symbol = symbol,
  })
  scope.maxSymbol = math.max(scope.maxSymbol, #symbol)
  scope.maxType = math.max(scope.maxType, #type)

  for eSubName2, eKey2 in eKey:gmatch('(%g-)_(%g+)$') do
    eSubName, eKey = eSubName2, eKey2
    enumScope = getEnumScope(eSubName, enumScope.subscopes)
  end

  insert(enumScope.keys, {
    name = eKey,
    symbol = symbol,
  })
  enumScope.maxKey = math.max(enumScope.maxKey, #eKey)
end

local function parseEnumConst (type, symbol, value)
  local eName, eKey = symbol:match('^(%g-)_(%g+)$')
  if not eName or not eKey then return end
  local enumScope = getEnumScope(eName, enums)

  for eSubName2, eKey2 in eKey:gmatch('(%g-)_(%g+)$') do
    eSubName, eKey = eSubName2, eKey2
    enumScope = getEnumScope(eSubName, enumScope.subscopes)
  end

  insert(enumScope.keys, {
    name = eKey,
    symbol = symbol,
    value = value,
  })
  enumScope.maxKey = math.max(enumScope.maxKey, #eKey)
end

local function parseMethod (retType, symbol, args)
  local sName, mName = symbol:match('^(%g-)_(%g+)$')
  if not sName or not mName then return end
  local scope = getScope(sName)
  insert(scope.elems, {
    name = mName,
    kind = 'method',
    type = retType,
    args = args:gsub('%s+', ' '),
    symbol = symbol,
  })
  scope.maxName = math.max(scope.maxName, #mName)
  scope.maxSymbol = math.max(scope.maxSymbol, #symbol)
  scope.maxType = math.max(scope.maxType, #retType)
end

local function parseTypedef (src, dst)
  maxTypedef = math.max(maxTypedef, #src)
  insert(typedefs, {
    src = src,
    dst = dst
  })
end

local function parseFuncPtr (retType, name, args)
  insert(funcPtrs, {
    name = name,
    retType = retType,
    args = args,
  })
  maxFuncPtr = math.max(maxFuncPtr, #name)
  maxFuncPtrRet = math.max(maxFuncPtrRet, #retType)
end

local function parseFile (path)
  local contents = io.open(path, 'rb'):read('*all')
  if contents:find('__FFI_IGNORE__') then return end

  -- Remove Comments
  contents = contents:gsub('//.-\n', '')
  contents = contents:gsub('/%*..-%*/', '')

  contents:gsub('PHX_API%s+(%g+)%s+(%g+)%s*%((.-)%);', parseMethod)
  contents:gsub('ENUM_T[%g ]- (%g+);', parseEnumDef)
  contents:gsub('PHX_API%s+const%s+(%g+)%s+(%g+);', parseEnum)
  contents:gsub('const%s+(%g+)%s+(%g+)%s+=%s+(%g+);', parseEnumConst)
  contents:gsub('OPAQUE_T%s+(%g+);', parseOpaque)
  contents:gsub('STRUCT_T%s+(%g+);', parseStructDef)
  contents:gsub('struct%s+(%g+)%s+{%s+(.-)%s+};', parseStruct)
  contents:gsub('typedef%s+([^%(%)]-)%s+(%g+)%s*;', parseTypedef)
  contents:gsub('typedef%s+(%g+)%s%(%*([%w_]+)%)%((.-)%)%s*;', parseFuncPtr)
end

local function flattenStruct (struct)
  local didFlatten = false
  for _, field in ipairs(struct.fields) do
    if isstruct[field.type] then
      didFlatten = true
      flattenStruct(getStruct(field.type))
    end
  end

  if didFlatten then
    local newFields = {}
    for _, field in ipairs(struct.fields) do
      if isstruct[field.type] and not field.isArray then
        local subFields = getStruct(field.type).fields
        for _, subField in ipairs(subFields) do
          insert(newFields, {
            name = field.name..subField.name,
            type = subField.type,
          })
        end
      else
        insert(newFields, field)
      end
    end
    struct.fields = newFields
  end
end

local function extraProcessing ()
  table.sort(structs, function (a, b) return a.name < b.name end)

  for name, _ in pairs(isstruct) do
    local struct = getStruct(name)
    -- HACK : I don't feel like rewriting RigidBody.lua
    if name ~= 'RigidBody' then
      flattenStruct(struct)
    end

    -- Issue Warnings
    if #struct.fields == 0 then
      warnf("Struct '%s' marked as transparent but no definition found.", struct.name)
    end
    if struct.nPreprocessor > 0 then
      warnf("Transparent struct '%s' contains preprocessor directives. "
        .."Cannot guarantee the Lua binding will be correct.", struct.name)
    end

    local function indexOf (t, predicateFn)
      for i = 1, #t do
        if predicateFn(t[i]) then return i end
      end
      return 0
    end

    -- Handle Dependencies
    for _, field in ipairs(struct.fields) do
      if isstruct[field.pType] then
        if field.isPtr then
          field.type = 'struct '..field.type
        else
          --[[ NOTE : Dependencies only occur when a field is an array of
                      another transparent struct. Later, if we flatten array
                      this can go away. ]]
          -- BUG : This should be done in a loop until no moves occur.
          insert(struct.dependencies, field.type)
          local iSelf = indexOf(structs, function (s) return s.name == struct.name end)
          local iDep  = indexOf(structs, function (s) return s.name == field.type end)
          if iDep > iSelf then insert(structs, iSelf, remove(structs, iDep)) end
        end
      end
      struct.maxFieldType = math.max(struct.maxFieldType, #field.type)
    end
  end
end

local function writeBindingsScope (scope)
  logf('Generating FFI bindings for %s...', scope.name)
  local file = io.open(format('%s/%s.lua', OUTPATH, scope.name), 'wb')
  assert(file)

  local lines = {}
  appendf(lines, [[-- %s %s]], scope.name, string.rep('-', 76 - #scope.name))
  appendf(lines, "local ffi = require('ffi')")
  appendf(lines, "local %s = require('ffi.%s').lib", LIBNAME, LIBNAME)
  appendf(lines, 'local %s\n', scope.name)

  do -- ffi cdefs
    if #scope.elems > 0 then
      appendf(lines, 'do -- C Definitions')
      appendf(lines, '  ffi.cdef [[')
      for _, elem in ipairs(scope.elems) do
        if elem.args then
          appendf(lines, '    %-*s %-*s (%s);',
            scope.maxType, elem.type, scope.maxSymbol, elem.symbol, elem.args)
        else
          appendf(lines, '    %-*s %s;',
            scope.maxType, elem.type, elem.symbol)
        end
      end
      appendf(lines, '  ]]')
      appendf(lines, 'end\n')
    end
  end

  local function appendAll(t1, t2)
    for i = 1, #t2 do insert(t1, t2[i]) end
  end

  do -- scope table
    appendf(lines, 'do -- Global Symbol Table')
    appendf(lines, '  %s = {', scope.name)

    -- enums
    local function appendEnumKeys (lines, indent, enumScope)
      for _, key in ipairs(enumScope.keys) do
        if key.value then
          appendf(lines, '%*s%-*s = %s,',
            2*indent, '', enumScope.maxKey, key.name, key.value)
        else
          appendf(lines, '%*s%-*s = %s.%s,',
            2*indent, '', enumScope.maxKey, key.name, LIBNAME, key.symbol)
        end
      end
    end
    local function appendEnumScope (lines, indent, enumScope)
      appendf(lines, '%*s%s = {', 2*indent, '', enumScope.name)
      appendEnumKeys(lines, indent + 1, enumScope)
      for _, subscope in ipairs(enumScope.subscopes) do
        appendEnumScope(lines, indent + 1, subscope)
      end
      appendf(lines, '%*s},', 2*indent, '')
    end

    for _, enumScope in ipairs(scope.enums) do
      if isenum[enumScope.name] then
        if enumScope.name == scope.name then
          appendEnumKeys(lines, 2, enumScope)
          for _, subscope in ipairs(enumScope.subscopes) do
            appendEnumScope(lines, 2, subscope)
          end
        else
          appendEnumScope(lines, 2, enumScope)
        end
      end
    end

    -- non-enums
    for _, elem in ipairs(scope.elems) do
      if elem.kind ~= 'enum' then
        appendf(lines, '    %-*s = %s.%s,',
          scope.maxName, elem.name, LIBNAME, elem.symbol)
        end
    end
    appendf(lines, '  }\n')

    if isstruct[scope.name] then
      appendf(lines, '  local mt = {')
      appendf(lines, '    __call  = function (t, ...) return %s_t(...) end,', scope.name)
      appendf(lines, '  }\n')
    end

    appendf(lines, '  if onDef_%s then onDef_%s(%s, mt) end',
      scope.name, scope.name, scope.name)
    appendf(lines, '  %s = setmetatable(%s, mt)', scope.name, scope.name)
    appendf(lines, 'end\n')
  end

  -- metatype
  if isopaque[scope.name] or isstruct[scope.name] then
    local function isMethod (elem, scopeName)
      -- NOTE : Matches functions that take the arg by value or by reference
      return elem.args and elem.args:find('^%s*' .. scopeName)
    end
    local function toMethodName (name)
      return name:sub(1, 1):lower() .. name:sub(2)
    end
    appendf(lines, 'do -- Metatype for class instances')
    appendf(lines, "  local t  = ffi.typeof('%s')", scope.name)
    appendf(lines, '  local mt = {')
    for _, elem in ipairs(scope.elems) do
      if elem.name == 'ToString' then
        appendf(lines, '    __tostring = function (self) return ffi.string(%s.%s(self)) end,', LIBNAME, elem.symbol)
      end
    end
    appendf(lines, '    __index = {')
    if isstruct[scope.name] then
      appendf(lines, '      %-*s = function (x) return %s_t(x) end,',
          scope.maxName, 'clone', scope.name)
    end
    for _, elem in ipairs(scope.elems) do
      if elem.name == 'Free' then
        appendf(lines, '      %-*s = function (self) return ffi.gc(self, %s.%s) end,',
          scope.maxName, 'managed', LIBNAME, elem.symbol)
      end
    end
    for _, elem in ipairs(scope.elems) do
      if isMethod(elem, scope.name) then
        appendf(lines, '      %-*s = %s.%s,',
          scope.maxName, toMethodName(elem.name), LIBNAME, elem.symbol)
      end
    end
    appendf(lines, '    },')
    appendf(lines, '  }\n')

    appendf(lines, '  if onDef_%s_t then onDef_%s_t(t, mt) end',
      scope.name, scope.name)
    appendf(lines, '  %s_t = ffi.metatype(t, mt)',
      scope.name, scope.name)
    appendf(lines, 'end\n')
  end

  appendf(lines, 'return %s\n', scope.name)

  lines = join(lines, '\n')
  file:write(lines)
  file:close()
end

local function writeBindingsInit ()
  local path = format('%s/%s.lua', OUTPATH, LIBNAME)
  local file = io.open(path, 'wb')
  if not file then errorf('Failed to open <%s> for writing', path) end

  local lines = {}
  appendf(lines, "local ffi = require('ffi')")
  appendf(lines, "local jit = require('jit')\n")
  appendf(lines, "local %s = {}", LIBNAME)

  do -- Typedefs
    appendf(lines, 'do -- Basic Typedefs')
    appendf(lines, '  ffi.cdef [[')
    for _, typedef in ipairs(typedefs) do
      appendf(lines, '    typedef %-*s %s;',
        maxTypedef, typedef.src, typedef.dst)
    end
    appendf(lines, '  ]]')
    appendf(lines, 'end\n')
  end

  do -- Function Pointers
    appendf(lines, 'do -- Function Pointer Typedefs')
    appendf(lines, '  ffi.cdef [[')
    for _, fPtr in ipairs(funcPtrs) do
      appendf(lines, '    typedef %-*s (*%-*s) (%s);',
        maxFuncPtrRet, fPtr.retType, maxFuncPtr, fPtr.name, fPtr.args)
    end
    appendf(lines, '  ]]')
    appendf(lines, 'end\n')
  end

  do -- Opaques
    appendf(lines, 'do -- Opaque Structs')
    appendf(lines, '  ffi.cdef [[')
    for _, opaque in ipairs(opaques) do
      appendf(lines, '    typedef struct %-*s {} %s;',
        maxOpaque, opaque, opaque)
    end
    appendf(lines, '  ]]\n')

    if true then -- Opaque List
      appendf(lines, '  %s.Opaques = {', LIBNAME)
      for _, opaque in ipairs(opaques) do
        appendf(lines, "    '%s',", opaque)
      end
      appendf(lines, '  }')
    end
    appendf(lines, 'end\n')
  end

  do -- Transparents
    appendf(lines, 'do -- Transparent Structs')
    appendf(lines, '  ffi.cdef [[')
    for i, struct in ipairs(structs) do
      if isstruct[struct.name] then
        appendf(lines, '    typedef struct %s {', struct.name)
        for _, field in ipairs(struct.fields) do
          appendf(lines, '      %-*s %s;',
            struct.maxFieldType, field.type, field.name)
        end
        appendf(lines, '    } %s;', struct.name)
        if i < #structs then appendf(lines, '') end
      end
    end
    appendf(lines, '  ]]\n')

    if true then -- Transparent List
      appendf(lines, '  %s.Structs = {', LIBNAME)
      for _, struct in ipairs(structs) do
        if isstruct[struct.name] then
          appendf(lines, "    '%s',", struct.name)
        end
      end
      appendf(lines, '  }')
    end

    appendf(lines, 'end\n')
  end

  do -- load dll
    appendf(lines, 'do -- Load Library')
    appendf(lines, "  local debug = __debug__ and 'd' or ''")
    appendf(lines, "  local arch = jit.arch == 'x86' and '32' or '64'")
    appendf(lines, "  local path = string.format('%s%%s%%s', arch, debug)", LIBNAME)
    appendf(lines, '  %s.lib = ffi.load(path, false)', LIBNAME)
    appendf(lines, "  assert(%s.lib, 'Failed to load %%s', path)", LIBNAME)
    -- appendf(lines, '  _G.%s = %s.lib', LIBNAME, LIBNAME)
    appendf(lines, 'end\n')
  end

  appendf(lines, 'return %s\n', LIBNAME)

  lines = join(lines, '\n')
  file:write(lines)
  file:close()
end

local function emptyDir (path)
  if lfs.attributes(path, 'mode') == 'directory' then
    local iter, dirObj = lfs.dir(path)
    while true do
      local fileName = iter(dirObj)
      if not fileName then break end
      if fileName:match('%.lua$') then
        os.remove(path..'/'..fileName)
      end
    end
  end
end

local function writeBindings ()
  lfs.mkdir(OUTPATH)
  emptyDir(OUTPATH)
  table.sort(scopes, function (a, b) return a.name < b.name end)
  table.sort(opaques)
  writeBindingsInit()
  for i = 1, #scopes do
    writeBindingsScope(scopes[i])
  end
end

local files = listdir(LIBPATH)
for i = 1, #files do parseFile(files[i]) end
extraProcessing()
writeBindings()

--[[ IMPORTANT
  TODO : Auto-wrap functions that take transparent structs by pointer (THIS WILL SAVE SO MUCH WORK IN BINDING EXTENSIONS)
  TODO : Flattening struct will introduce alignment issues if the alignment
         of the struct is not the same as the alignment of the first field
         within it.
  TODO : Better const* handling (robust parsing in struct fields and argument lists)
  TODO : Think about what to do with arrays of structs
--]]

-- TODO : Emit operators for *_Mul, Div, Add, and Sub
-- TODO : Don't interpret functions with a single out parameter as methods (e.g. Quat_Identity)
-- TODO : Can/should we build automatic ToString methods for structs?
-- TODO : Don't emit bindings for empty scopes (and warn?)
-- TODO : Use Log.Warning/Error? (Also LTEdit)
-- TODO : Parse normal typedefs and enums separately
-- TODO : Find some way to associate a metatype with typedefs of primitives
-- TODO : It would be better to parse the preprocessed files
-- TODO : Silent failure when it doesn't find libphx
-- TODO : Better verbose option
-- TODO : Success/failure output
-- TODO : Better indent handling

--[[ NOTE : It would be nice to define transparent structs in their respective
            binding files, rather than in libphx.ffi.lua, but we'd have to do
            extra work to ensure dependencies are loaded first. ]]

--[[ NOTE : Metatables associated with FFI metatypes are NOT modifiable once
            created. Extension scripts must be loaded before bindings are
            created and applied during creation. ]]
