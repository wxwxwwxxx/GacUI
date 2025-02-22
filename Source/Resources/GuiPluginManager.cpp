#include "GuiPluginManager.h"

namespace vl
{
	namespace presentation
	{
		using namespace collections;

/***********************************************************************
GuiPluginManager
***********************************************************************/

		class GuiPluginManager : public Object, public IGuiPluginManager
		{
		protected:
			List<Ptr<IGuiPlugin>>				plugins;
			bool								loaded;
		public:
			GuiPluginManager()
				:loaded(false)
			{
			}

			~GuiPluginManager()
			{
				Unload();
			}

			void AddPlugin(Ptr<IGuiPlugin> plugin)override
			{
				CHECK_ERROR(!loaded, L"GuiPluginManager::AddPlugin(Ptr<IGuiPlugin>)#Load function has already been executed.");
				auto name = plugin->GetName();
				if (name != L"")
				{
					for (auto plugin : plugins)
					{
						CHECK_ERROR(plugin->GetName() != name, L"GuiPluginManager::AddPlugin(Ptr<IGuiPlugin>)#Duplicated plugin name.");
					}
				}
				plugins.Add(plugin);
			}

			void Load()override
			{
				CHECK_ERROR(!loaded, L"GuiPluginManager::AddPlugin(Ptr<IGuiPlugin>)#Load function has already been executed.");
				loaded=true;

				SortedList<WString> loaded;
				Group<WString, WString> loading;
				Dictionary<WString, Ptr<IGuiPlugin>> pluginsToLoad;
				for (auto plugin : plugins)
				{
					auto name = plugin->GetName();
					pluginsToLoad.Add(name, plugin);
					List<WString> dependencies;
					plugin->GetDependencies(dependencies);
					for (auto dependency : dependencies)
					{
						loading.Add(name, dependency);
					}
				}

				while (pluginsToLoad.Count() > 0)
				{
					vint count = pluginsToLoad.Count();
					{
						// TODO: (enumerable) foreach:alterable(reversed) on dictionary
						for (auto [name, index] : indexed(pluginsToLoad.Keys()))
						{
							if (!loading.Keys().Contains(name))
							{
								// TODO: (enumerable) foreach:alterable(reversed) on group
								for (vint i = loading.Count() - 1; i >= 0; i--)
								{
									loading.Remove(loading.Keys()[i], name);
								}
								loaded.Add(name);

								auto plugin = pluginsToLoad.Values()[index];
								pluginsToLoad.Remove(name);
								plugin->Load();
								break;
							}
						}
					}
					if (count == pluginsToLoad.Count())
					{
						WString message;
						for (auto plugin : pluginsToLoad.Values())
						{
							message += L"Cannot load plugin \"" + plugin->GetName() + L"\" because part of its dependencies are not ready:";
							List<WString> dependencies;
							plugin->GetDependencies(dependencies);
							bool first = true;
							for (auto dependency : dependencies)
							{
								if (!loaded.Contains(dependency))
								{
									message += L" \"" + dependency + L"\";";
								}
							}
							message += L"\r\n";
						}
						throw Exception(message);
					}
				}
			}

			void Unload()override
			{
				CHECK_ERROR(loaded, L"GuiPluginManager::AddPlugin(Ptr<IGuiPlugin>)#Load function has not been executed.");
				loaded=false;
				for (auto plugin : plugins)
				{
					plugin->Unload();
				}
			}

			bool IsLoaded()override
			{
				return loaded;
			}
		};

/***********************************************************************
Helpers
***********************************************************************/

		GuiPluginDescriptor* firstPluginDescriptor = nullptr;
		GuiPluginDescriptor** lastPluginDescriptor = &firstPluginDescriptor;
		IGuiPluginManager* pluginManager = nullptr;

		IGuiPluginManager* GetPluginManager()
		{
			if (!pluginManager)
			{
				pluginManager = new GuiPluginManager;

				auto current = firstPluginDescriptor;
				while (current)
				{
					pluginManager->AddPlugin(current->CreatePlugin());
					current = current->next;
				}
			}
			return pluginManager;
		}

		void RegisterPluginDescriptor(GuiPluginDescriptor* pluginDescriptor)
		{
			CHECK_ERROR(!pluginManager, L"vl::presentation::RegisterPluginDescriptor(GuiPluginDescriptor*)#This function should be called before calling GetPluginManager.");
			*lastPluginDescriptor = pluginDescriptor;
			lastPluginDescriptor = &pluginDescriptor->next;
		}

		void DestroyPluginManager()
		{
			if (pluginManager)
			{
				delete pluginManager;
				pluginManager = nullptr;
			}
		}
	}
}