#include "nitrogen/nodeutil.hpp"

void event_emitter::on(std::string const& name, v8::Handle<v8::Function> callback)
{
	std::unique_lock<std::mutex> lock(mtx_);

	v8::Isolate* isolate = callback->GetIsolate();
	v8::HandleScope scope(isolate);

	auto cb = v8::UniquePersistent<v8::Function>(isolate, callback);

	auto range = handlers_.equal_range(name);
	range.first = std::find_if(range.first, range.second,
		[&cb](auto const& handler)
	{
		return handler.second == cb;
	});
	if (range.first == range.second)
	{
		handlers_.insert(range.second, std::make_pair(name, cb.Pass()));
	}
}

void event_emitter::off(std::string const& name, v8::Handle<v8::Function> callback)
{
	std::unique_lock<std::mutex> lock(mtx_);

	auto range = handlers_.equal_range(name);
	if (name.empty())
	{
		range.first = handlers_.begin();
		range.second = handlers_.end();
	}
	else if (!callback.IsEmpty())
	{
		v8::Isolate* isolate = callback->GetIsolate();
		v8::HandleScope scope(isolate);
		auto cb = v8::UniquePersistent<v8::Function>(isolate, callback);
		range.first = std::find_if(range.first, range.second,
			[&cb](auto const& handler)
		{
			return handler.second == cb;
		});
	}
	handlers_.erase(range.first, range.second);
}

bool event_emitter::has(std::string const& name) const
{
	std::unique_lock<std::mutex> lock(mtx_);
	return handlers_.find(name) != handlers_.end();
}

void event_emitter::emit(std::string const& name,
	v8::Handle<v8::Object> recv, int argc, v8::Handle<v8::Value> argv[])
{
	if (!recv.IsEmpty())
	{
		std::unique_lock<std::mutex> lock(mtx_);

		v8::Isolate* isolate = recv->GetIsolate();
		v8::HandleScope scope(isolate);

		auto range = handlers_.equal_range(name);
		for (auto it = range.first; it != range.second; ++it)
		{
			it->second.Get(isolate)->Call(recv, argc, argv);
		}
	}
}
