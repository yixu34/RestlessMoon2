#ifndef RESOURCECACHE_HPP
#define RESOURCECACHE_HPP

//
// Templated cache class for all resource types.  
// NOTE:  Instantiate this class using the resource's type, not its pointer
// type or its shared-pointer type.  i.e. write ResourceCache<Mesh>, not 
// ResourceCache<Mesh *> or ResourceCache<MeshPtr>.  This way, we can change
// our internal storage type without having to change the way our caches
// are used.
//

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <algorithm>

// TODO:  Maintain a LRU list of resources, so that we can get rid of old
// resources and make room for new ones.
template <typename T>
class ResourceCache
{
	typedef boost::shared_ptr<T> TPtr;
public:
	ResourceCache()  {}
	~ResourceCache() { clear(); }

	TPtr get(const std::string &key);
	void clear();

private:
	typedef std::map<std::string, TPtr> ResourcePool;
	ResourcePool _contents;
};

template <typename T>
boost::shared_ptr<T> ResourceCache<T>::get(const std::string &key)
{
	ResourcePool::iterator it = _contents.find(key);
	if (it == _contents.end())
	{
		TPtr toGet = TPtr(new T());
		if (!toGet->load(key))
			return TPtr();

		_contents[key] = toGet;
		return toGet;
	}
	else
		return it->second;
}

template <typename T>
void ResourceCache<T>::clear()
{
	for (ResourcePool::iterator it = _contents.begin(); 
		 it != _contents.end(); it++)
	{
		it->second->unload();
	}
	_contents.clear();
}

#endif
