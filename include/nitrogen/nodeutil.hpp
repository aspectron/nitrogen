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
NITROGEN_API v8::Handle<v8::Value> require(v8::Handle<v8::Object> module, char const* name);

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
