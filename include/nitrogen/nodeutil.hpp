#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

#include <v8.h>
#include <uv.h>

#include "nitrogen/nitrogen.hpp"

/// Call a function/lambda f as the Node task in its thread
template<typename F>
void call_in_node(F&& f)
{
	class node_task : uv_async_t
	{
	public:
		static void run(F&& f)
		{
			node_task* task = new node_task(std::forward<F>(f));
			uv_async_init(uv_default_loop(), task, &node_task::call);
			task->data = task;
			uv_async_send(task);
		}
	private:
		F f;

		explicit node_task(F&& f)
			: f(std::move(f))
		{
		}

		// uv_async_t implementation
		static void call(uv_async_t* handle)
		{
			node_task* task = static_cast<node_task*>(handle->data);
			task->f();
			uv_close((uv_handle_t*)handle, &node_task::destroy);
		}

		static void destroy(uv_handle_t* handle)
		{
			node_task* task = static_cast<node_task*>(handle->data);
			delete task;
		}
	};

	node_task::run(std::forward<F>(f));
}

/// Call module.require(name) from C++ code
inline v8::Handle<v8::Value> require(v8::Handle<v8::Object> module, char const* name)
{
	v8::Isolate* isolate = module->GetIsolate();
	v8::EscapableHandleScope scope(isolate);
	v8::Local<v8::Value> require_name = v8::String::NewFromUtf8(isolate, "require");
	v8::Local<v8::Value> module_name = v8::String::NewFromUtf8(isolate, name);
	v8::Local<v8::Function> require = module->Get(require_name).As<v8::Function>();
	v8::Local<v8::Value> result = require->Call(module, 1, &module_name);
	return scope.Escape(result);
}

/// List of persistent handles to loaded modules
class required_modules
{
public:
	required_modules() = default;
	required_modules(required_modules const&) = delete;
	required_modules& operator=(required_modules const&) = delete;
	~required_modules() { unload(); }

	/// Load list of modules
	void required_modules::load(v8::Handle<v8::Object> module, std::initializer_list<char const*> names)
	{
		unload();
		modules_.reserve(names.size());
		for (auto name : names)
		{
			modules_.emplace_back(module->GetIsolate(), require(module, name));
		}
	}

	/// Unload all modules
	void required_modules::unload()
	{
		std::for_each(modules_.rbegin(), modules_.rend(),
			[](persistent& module) { module.Reset(); });
		modules_.clear();
	}

private:
	using persistent = v8::UniquePersistent<v8::Value>;
	std::vector<persistent> modules_;
};

/// Event emitter
class NITROGEN_API event_emitter
{
public:
	void on(std::string const& name, v8::Handle<v8::Function> callback);
	void off(std::string const& name, v8::Handle<v8::Function> callback);

	bool has(std::string const& name) const;

	void emit(std::string const& name,
		v8::Handle<v8::Object> recv, int argc, v8::Handle<v8::Value> argv[]);

private:
	mutable std::mutex mtx_;
	std::unordered_multimap<std::string, v8::UniquePersistent<v8::Function>> handlers_;
};
