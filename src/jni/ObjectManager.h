#ifndef OBJECTMANAGER_H_
#define OBJECTMANAGER_H_

#include "v8.h"
#include "JEnv.h"
#include "ArgsWrapper.h"
#include "JSInstanceInfo.h"
#include "DirectBuffer.h"
#include "LRUCache.h"
#include <unordered_map>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <string>

namespace tns
{
	class ObjectManager
	{
	public:
		ObjectManager();

		static jweak GetJavaObjectByJsObjectStatic(const v8::Handle<v8::Object>& object);

		jweak GetJavaObjectByJsObject(const v8::Handle<v8::Object>& object);

		jweak GetJavaObjectByID(uint32_t javaObjectID);

		void UpdateCache(int objectID, jobject obj);

		jclass GetJavaClass(const v8::Handle<v8::Object>& instance);

		int GetOrCreateObjectId(jobject object);

		static v8::Handle<v8::Object> GetJsObjectByJavaObjectStatic(int javaObjectID);

		v8::Local<v8::Object> GetJsObjectByJavaObject(int javaObjectID);

		static v8::Handle<v8::Object> CreateJSProxyInstanceStatic(jint javaObjectID, const std::string& typeName);

		v8::Handle<v8::Object> CreateJSProxyInstance(jint javaObjectID, const std::string& typeName);

		v8::Handle<v8::Object> CreateJSProxyInstance(jint javaObjectID, const std::string& typeName, jobject instance);

		void Link(const v8::Handle<v8::Object>& object, uint32_t javaObjectID, jclass clazz);

		std::string GetClassName(jobject javaObject);

		std::string GetClassName(jclass clazz);

		int GenerateNewObjectID();

	private:
		struct ObjectWeakCallbackState
		{
			ObjectWeakCallbackState(ObjectManager *_thisPtr, JSInstanceInfo *_jsInfo, v8::Persistent<v8::Object> *_target)
				: thisPtr(_thisPtr), jsInfo(_jsInfo), target(_target)
			{
			}

			ObjectManager *thisPtr;
			JSInstanceInfo *jsInfo;
			v8::Persistent<v8::Object> *target;
		};

		struct GarbageCollectionInfo
		{
			GarbageCollectionInfo(int _numberOfGC)
				: numberOfGC(_numberOfGC)
			{
			}
			std::vector<v8::Persistent<v8::Object>*> markedForGC;
			int numberOfGC;
		};

		class PersistentObjectIdSet
		{
		public:
			PersistentObjectIdSet() { /* TODO: use functors */ }

			void clear()
			{
				m_POs.clear();
				m_IDs.clear();
			}

			void insert(v8::Persistent<v8::Object>* po, int javaObjectId)
			{
				m_POs.insert(po);
				m_IDs.insert(javaObjectId);
			}

			bool contains(v8::Persistent<v8::Object>* po)
			{
				return m_POs.find(po) != m_POs.end();
			}

			std::set<v8::Persistent<v8::Object>*> m_POs;
			std::set<int> m_IDs;
		};

		struct PersistentObjectIdPair
		{
			PersistentObjectIdPair(v8::Persistent<v8::Object>* _po, int _javaObjectId)
				: po(_po), javaObjectId(_javaObjectId)
			{
			}
			v8::Persistent<v8::Object> *po;
			int javaObjectId;
		};

		JSInstanceInfo* GetJSInstanceInfo(const v8::Handle<v8::Object>& object);

		void ReleaseJSInstance(v8::Persistent<v8::Object> *po, JSInstanceInfo *jsInstanceInfo);

		void ReleaseRegularObjects();

		void MakeRegularObjectsWeak(const std::set<int>& instances, DirectBuffer& inputBuff);

		void MakeImplObjectsWeak(const std::vector<PersistentObjectIdPair>& instances, DirectBuffer& inputBuff);

		void CheckWeakObjectsAreAlive(const std::vector<PersistentObjectIdPair>& instances, DirectBuffer& inputBuff, DirectBuffer& outputBuff);

		v8::Handle<v8::Object> CreateJSProxyInstanceHelper(jint javaObjectID, const std::string& typeName, jclass clazz);

		static void JSObjectWeakCallbackStatic(const v8::WeakCallbackData<v8::Object, ObjectWeakCallbackState>& data);

		void JSObjectWeakCallback(v8::Isolate *isolate, ObjectWeakCallbackState *callbackState);

		std::string JniClassPathToCanonicalName(const std::string& jniClassPath);

		bool HasImplObject(v8::Isolate *isolate, const v8::Local<v8::Object>& obj);

		void MarkReachableObjects(v8::Isolate *isolate, const v8::Local<v8::Object>& obj);

		void OnGcStarted(v8::GCType type, v8::GCCallbackFlags flags);

		void OnGcFinished(v8::GCType type, v8::GCCallbackFlags flags);

		static void OnGcStartedStatic(v8::GCType type, v8::GCCallbackFlags flags);

		static void OnGcFinishedStatic(v8::GCType type, v8::GCCallbackFlags flags);

		jobject GetJavaObjectByIDImpl(uint32_t javaObjectID);

		static jweak NewWeakGlobalRefCallback(const int& javaObjectID, void *state);

		static void DeleteWeakGlobalRefCallback(const jweak& object, void *state);

		int m_numberOfGC;

		std::stack<GarbageCollectionInfo> m_markedForGC;

		std::map<int, v8::Persistent<v8::Object>*> idToObject;

		PersistentObjectIdSet m_released;

		std::set<uint8_t*> m_visited;

		LRUCache<int, jweak, std::tr1::unordered_map> m_cache;

		std::set<v8::Persistent<v8::Object>*> m_visitedPOs;
		std::vector<PersistentObjectIdPair> m_implObjWeak;
		std::vector<PersistentObjectIdPair> m_implObjStrong;

		volatile int m_currentObjectId;

		DirectBuffer m_buff;

		DirectBuffer m_outBuff;

		jclass PlatformClass;

		jmethodID GET_JAVAOBJECT_BY_ID_METHOD_ID;

		jmethodID GET_OR_CREATE_JAVA_OBJECT_ID_METHOD_ID;

		jmethodID MAKE_INSTANCE_WEAK_BATCH_METHOD_ID;

		jmethodID CHECK_WEAK_OBJECTS_ARE_ALIVE_METHOD_ID;

		static ObjectManager *instance;
	};
}

#endif /* OBJECTMANAGER_H_ */
