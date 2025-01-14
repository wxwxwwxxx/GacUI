#include "GuiGraphicsRepeatComposition.h"
#include "../Controls/Templates/GuiControlTemplates.h"

namespace vl
{
	namespace presentation
	{
		namespace compositions
		{
			using namespace reflection::description;
			using namespace collections;
			using namespace controls;
			using namespace elements;

/***********************************************************************
GuiRepeatCompositionBase
***********************************************************************/

			void GuiRepeatCompositionBase::OnItemChanged(vint index, vint oldCount, vint newCount)
			{
				if (itemTemplate && itemSource)
				{
					for (vint i = oldCount - 1; i >= 0; i--)
					{
						RemoveItem(index + i);
					}

					for (vint i = 0; i < newCount; i++)
					{
						InstallItem(index + i);
					}
				}
			}

			void GuiRepeatCompositionBase::RemoveItem(vint index)
			{
				GuiItemEventArgs arguments(dynamic_cast<GuiGraphicsComposition*>(this));
				arguments.itemIndex = index;
				ItemRemoved.Execute(arguments);

				auto item = RemoveRepeatComposition(index);
				SafeDeleteComposition(item);
			}

			void GuiRepeatCompositionBase::InstallItem(vint index)
			{
				auto source = itemSource->Get(index);
				auto templateItem = itemTemplate(source);
				auto item = InsertRepeatComposition(index);

				templateItem->SetAlignmentToParent(Margin(0, 0, 0, 0));
				templateItem->SetContext(itemContext);
				item->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
				item->AddChild(templateItem);

				GuiItemEventArgs arguments(dynamic_cast<GuiGraphicsComposition*>(this));
				arguments.itemIndex = index;
				ItemInserted.Execute(arguments);
			}

			void GuiRepeatCompositionBase::ClearItems()
			{
				vint count = GetRepeatCompositionCount();
				for (vint i = count - 1; i >= 0; i--)
				{
					RemoveItem(i);
				}
			}

			void GuiRepeatCompositionBase::InstallItems()
			{
				if (itemTemplate && itemSource)
				{
					vint count = itemSource->GetCount();
					for (vint i = 0; i < count; i++)
					{
						InstallItem(i);
					}
				}
			}

			GuiRepeatCompositionBase::GuiRepeatCompositionBase()
			{
			}

			GuiRepeatCompositionBase::~GuiRepeatCompositionBase()
			{
				if (itemChangedHandler)
				{
					itemSource.Cast<IValueObservableList>()->ItemChanged.Remove(itemChangedHandler);
				}
			}

			GuiRepeatCompositionBase::ItemStyleProperty GuiRepeatCompositionBase::GetItemTemplate()
			{
				return itemTemplate;
			}

			void GuiRepeatCompositionBase::SetItemTemplate(ItemStyleProperty value)
			{
				ClearItems();
				itemTemplate = value;
				if (itemTemplate && itemSource)
				{
					InstallItems();
				}
			}

			Ptr<IValueEnumerable> GuiRepeatCompositionBase::GetItemSource()
			{
				return itemSource;
			}

			void GuiRepeatCompositionBase::SetItemSource(Ptr<IValueEnumerable> value)
			{
				if (value != itemSource)
				{
					if (itemChangedHandler)
					{
						itemSource.Cast<IValueObservableList>()->ItemChanged.Remove(itemChangedHandler);
					}

					ClearItems();
					itemSource = value.Cast<IValueList>();
					if (!itemSource && value)
					{
						itemSource = IValueList::Create(GetLazyList<Value>(value));
					}

					if (itemTemplate && itemSource)
					{
						InstallItems();
					}
					if (auto observable = itemSource.Cast<IValueObservableList>())
					{
						itemChangedHandler = observable->ItemChanged.Add(this, &GuiRepeatCompositionBase::OnItemChanged);
					}
				}
			}

			description::Value GuiRepeatCompositionBase::GetContext()
			{
				return itemContext;
			}

			void GuiRepeatCompositionBase::SetContext(const description::Value& value)
			{
				if (itemContext != value)
				{
					itemContext = value;
					vint count = GetRepeatCompositionCount();
					for (vint i = 0; i < count; i++)
					{
						auto rc = GetRepeatComposition(i);
						auto it = dynamic_cast<templates::GuiTemplate*>(rc->Children()[0]);
						it->SetContext(itemContext);
					}

					GuiEventArgs arguments(dynamic_cast<GuiGraphicsComposition*>(this));
					ContextChanged.Execute(arguments);
				}
			}

/***********************************************************************
GuiRepeatStackComposition
***********************************************************************/

			vint GuiRepeatStackComposition::GetRepeatCompositionCount()
			{
				return GetStackItems().Count();
			}

			GuiGraphicsComposition* GuiRepeatStackComposition::GetRepeatComposition(vint index)
			{
				return GetStackItems()[index];
			}

			GuiGraphicsComposition* GuiRepeatStackComposition::InsertRepeatComposition(vint index)
			{
				CHECK_ERROR(0 <= index && index <= GetStackItems().Count(), L"GuiRepeatStackComposition::InsertRepeatComposition(vint)#Index out of range.");
				auto item = new GuiStackItemComposition;
				InsertStackItem(index, item);
				return item;
			}

			GuiGraphicsComposition* GuiRepeatStackComposition::RemoveRepeatComposition(vint index)
			{
				auto item = GetStackItems()[index];
				RemoveChild(item);
				return item;
			}

			GuiRepeatStackComposition::GuiRepeatStackComposition()
			{
				ItemInserted.SetAssociatedComposition(this);
				ItemRemoved.SetAssociatedComposition(this);
				ContextChanged.SetAssociatedComposition(this);
			}

			GuiRepeatStackComposition::~GuiRepeatStackComposition()
			{
			}

/***********************************************************************
GuiRepeatFlowComposition
***********************************************************************/

			vint GuiRepeatFlowComposition::GetRepeatCompositionCount()
			{
				return GetFlowItems().Count();
			}

			GuiGraphicsComposition* GuiRepeatFlowComposition::GetRepeatComposition(vint index)
			{
				return GetFlowItems()[index];
			}

			GuiGraphicsComposition* GuiRepeatFlowComposition::InsertRepeatComposition(vint index)
			{
				CHECK_ERROR(0 <= index && index <= GetFlowItems().Count(), L"GuiRepeatStackComposition::InsertRepeatComposition(vint)#Index out of range.");
				auto item = new GuiFlowItemComposition;
				InsertFlowItem(index, item);
				return item;
			}

			GuiGraphicsComposition* GuiRepeatFlowComposition::RemoveRepeatComposition(vint index)
			{
				auto item = GetFlowItems()[index];
				RemoveChild(item);
				return item;
			}

			GuiRepeatFlowComposition::GuiRepeatFlowComposition()
			{
				ItemInserted.SetAssociatedComposition(this);
				ItemRemoved.SetAssociatedComposition(this);
				ContextChanged.SetAssociatedComposition(this);
			}

			GuiRepeatFlowComposition::~GuiRepeatFlowComposition()
			{
			}
		}
	}
}