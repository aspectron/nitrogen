#!/usr/bin/env node

var path = require('path')

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

function print_includes(modules)
{
	modules.forEach(function(module)
	{
		print(path.join(module, '../../../include'))
	})
}

function print_libs(modules)
{
	modules.forEach(function(module)
	{
		print(process.platform === 'win32'? replace_ext(module, '.lib') : module)
	})
}

function print_gyps(modules)
{
	modules.forEach(function(module) {
		print(path.join(module, '../../../binding.gyp'))
	})
}

function main(args)
{
	var modes =
	{
		'include': print_includes,
		'lib': print_libs,
		'gyp': print_gyps,
	}

	var mode = args[0] || ''
	var handler = modes[mode]
	if (!handler)
	{
		console.error("Unknown mode " + mode + ", use one of these: " + Object.keys(modes))
		return
	}
	var modules = args.slice(1).map(function(name) { return require.resolve(name) });
	handler(modules)
}

main(process.argv.slice(2))
