#!/usr/bin/env node

var path = require('path')

var mode = process.argv[2]
var modules = process.argv.slice(3).map(function(name) { return require.resolve(name) });

function replace_ext(filename, new_ext)
{
	return path.join(
		path.dirname(filename),
		path.basename(filename, path.extname(filename)) + new_ext)
}

function print(filename)
{
	if (process.platform === 'win32')
	{
		filename = filename.split(path.sep).join(path.posix.sep)
	}
	console.log(filename)
}

switch (mode)
{
case 'include':
	modules.forEach(function(module) {
		print(path.join(module, '../../../include'))
	})
	break
case 'lib':
	modules.forEach(function(module) {
		print(process.platform === 'win32'? replace_ext(module, '.lib') : module)
	})
	break
default:
	console.error("Unknown mode " + mode + ", use `include` or `lib`")
	process.exit(-1)
}