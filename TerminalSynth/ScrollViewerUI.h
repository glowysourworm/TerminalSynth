#pragma once

#ifndef SCROLL_VIEWER_UI_H
#define SCROLL_VIEWER_UI_H

#include "ModelUI.h"
#include "UIBase.h"
#include <algorithm>
#include <cmath>
#include <concepts>
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

/// <summary>
/// Concept for enforcing a constraint on the UI model classes. They must inherit from
/// ModelUI.
/// </summary>
template<class TModel>
concept IsUIModel = std::convertible_to<TModel*, ModelUI*>;

/// <summary>
/// Concept that ensures that the constraint on the component class is to inherit
/// from UIBase*
/// </summary>
template<class T, class TModel>
concept IsUIBase = std::convertible_to<T*, UIBase<TModel>*>;

/// <summary>
/// Container class for UIBase components. The template constraint is for the class T, and
/// class template value TModel, to be properly inherited from UIBase component(s).
/// </summary>
/// <typeparam name="TModel">Model value type name</typeparam>
/// <typeparam name="T">UIBase* class (must inherit from UIBase)</typeparam>
template<class TModel, IsUIBase<TModel> T>
class ScrollViewerUI
{
public:

	ScrollViewerUI(const std::string& title, const ftxui::Color& titleColor, float scrollDeltaY = 0.05);
	~ScrollViewerUI();

	ftxui::Component GetComponent();

	/// <summary>
	/// Creates a new UI instance and applies the model to it using the default contructor. Also,
	/// calls initialize on the UIBase* instance. If there is an instance (based on == equality)
	/// already in the ScrollViewerUI, then there will be an exception thrown.
	/// </summary>
	void AddUI(const TModel& model);

	/// <summary>
	/// Removes current UI instance based on model == equality. If there is no model in the 
	/// ScrollViewerUI, then an exception is thrown.
	/// </summary>
	void RemoveUI(const TModel& model);
	
	// UIBase Functions:  There could be another component type which is a container for 
	//					  more UIBase components.
	void UpdateComponent();
	void ServicePendingAction();
	void Tick();

	void FromUI(const std::string& name, TModel& destination);
	void FromUI(const std::string& name, TModel* destination);
	void ToUI(const std::string& name, const TModel& source);
	void ToUI(const std::string& name, const TModel* source);
	int GetUICount() const;
	std::string GetName(int index) const;
	T* GetUI(const std::string& name) const;

	void MoveUp(const std::string& name);
	void MoveDown(const std::string& name);

	bool GetDirty() const;
	bool GetDirty(const std::string& name) const;
	void ClearDirty();
	void ClearDirty(const std::string& name);

	bool HasPendingAction(const std::string& name) const;
	bool HasPendingAction() const;
	void ClearPendingAction(const std::string& name);
	void ClearPendingAction();

private:

	/// <summary>
	/// Re-sorts maps based on order, and rebuilds UI component
	/// </summary>
	void RebuildUI();

private:

	ftxui::Component _component;
	ftxui::Component _listComponent;

	std::string* _title;
	ftxui::Color* _titleColor;

	bool* _isMouseOver;
	float* _scrollY;
	float _scrollDeltaY;

	std::map<std::string, T*>* _uiComponents;			// T : UIBase
	std::map<std::string, TModel*>* _uiModels;			// TModel (must have copy constructor!)
	//std::map<int, TModel*>* _uiModelOrders;
};

template<class TModel, IsUIBase<TModel> T>
ScrollViewerUI<TModel, T>::ScrollViewerUI(const std::string& title, const ftxui::Color& titleColor, float scrollDeltaY)
{
	_scrollY = new float(0);
	_scrollDeltaY = scrollDeltaY;
	_isMouseOver = new bool(false);
	_uiComponents = new std::map<std::string, T*>();
	_uiModels = new std::map<std::string, TModel*>();
	//_uiModelOrders = new std::map<int, TModel*>();

	_title = new std::string(title);
	_titleColor = new ftxui::Color(titleColor);

	// Slider bound to the position of the list
	ftxui::SliderOption<float> sliderOptions;
	sliderOptions.value = _scrollY;
	sliderOptions.min = 0.f;
	sliderOptions.max = 1.f;
	sliderOptions.increment = 0.1f;
	sliderOptions.direction = ftxui::Direction::Down;
	sliderOptions.color_active = ftxui::Color::BlueLight;
	sliderOptions.color_inactive = ftxui::Color::White;

	// This ftxui::Make (std::make_shared) call seems to be ok. Some of the components are less 
	// brittle when they have a place in the private memory of the class. I'm still not sure how
	// and when the std::shared_ptr memory is colletced; and the destructors are called.
	//
	auto scrollBarY = ftxui::Slider(sliderOptions);

	_listComponent = ftxui::Container::Vertical({});

	_component = ftxui::Container::Vertical(
	{
		ftxui::Renderer([&] { return ftxui::text(*_title) | ftxui::color(*_titleColor); }),
		ftxui::Renderer([&] { return ftxui::separator(); }),

		ftxui::Container::Horizontal({

			// Must render the list to add the focused relative position offset
			ftxui::Renderer(_listComponent, [&] {
				return _listComponent->Render() | ftxui::focusPositionRelative(0, *_scrollY) | ftxui::yframe;
			}) | ftxui::flex_grow,

			scrollBarY | ftxui::yframe | ftxui::align_right
		})

	}) | ftxui::CatchEvent([&](ftxui::Event event)
	{
		if (event.mouse().button == ftxui::Mouse::Button::WheelUp && *_isMouseOver)
		{
			// Set Mouse Wheel (clipped [0,1])
			*_scrollY = fminf(fmaxf(*_scrollY - _scrollDeltaY, 0), 1);
		}
		else if (event.mouse().button == ftxui::Mouse::Button::WheelDown && *_isMouseOver)
		{
			// Set Mouse Wheel (clipped [0,1])
			*_scrollY = fminf(fmaxf(*_scrollY + _scrollDeltaY, 0), 1);
		}

		// Pass through
		if (event.is_mouse())
			return false;

		// Cancel keyboard events
		return true;

	}) | ftxui::Hoverable(_isMouseOver) | ftxui::border;
}

template<class TModel, IsUIBase<TModel> T>
ScrollViewerUI<TModel, T>::~ScrollViewerUI()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		delete iter->second;
	}
	for (auto iter = _uiModels->begin(); iter != _uiModels->end(); ++iter)
	{
		delete iter->second;
	}

	delete _title;
	delete _titleColor;
	delete _isMouseOver;
	delete _scrollY;
	delete _uiComponents;
	delete _uiModels;
	//delete _uiModelOrders;
}

template<class TModel, IsUIBase<TModel> T>
ftxui::Component ScrollViewerUI<TModel, T>::GetComponent()
{
	return ftxui::Renderer(_component, [&] {
		return _component->Render();
	});
}

template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::AddUI(const TModel& model)
{
	// Existing Model
	if (_uiModels->contains(model.GetName()))
	{
		if (model == *_uiModels->at(model.GetName()))
			throw new std::exception("Existing model matched equality check! Please be sure not to add duplicate model values!");
	}

	// (MEMORY!) Create TModel* model using copy constructor
	TModel* uiModel = new TModel(model);

	// (MEMORY!) Create UIBase* component
	T* uiComponent = new T(model);

	// -> UIBase::Initialize
	uiComponent->Initialize(model);

	// Keep these in the ui tree
	_uiComponents->insert(std::make_pair(model.GetName(), uiComponent));
	_uiModels->insert(std::make_pair(model.GetName(), uiModel));
	//_uiModelOrders->insert(std::make_pair(model.GetOrder(), uiModel));
	_listComponent->Add(uiComponent->GetComponent());
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::RemoveUI(const TModel& model)
{
	bool existingModel = false;

	// Existing Model?
	if (!_uiModels->contains(model.GetName()))
		throw new std::exception("Existing model not found via equality check! Please be sure that model has been added!");

	// MEMORY! (~ScrollViewerUI)
	delete _uiComponents->at(model.GetName());
	delete _uiModels->at(model.GetName());

	// Remove
	_uiComponents->erase(model.GetName());
	_uiModels->erase(model.GetName());
	//_uiModelOrders->erase(model.GetOrder());
	
	this->RebuildUI();
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::UpdateComponent()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		iter->second->UpdateComponent();
	}
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ServicePendingAction()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		iter->second->ServicePendingAction();
	}
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::Tick()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		iter->second->Tick();
	}
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::FromUI(const std::string& name, TModel& destination)
{
	_uiComponents->at(name)->FromUI(destination);
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::FromUI(const std::string& name, TModel* destination)
{
	_uiComponents->at(name)->FromUI(destination);
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ToUI(const std::string& name, const TModel& source)
{
	_uiComponents->at(name)->ToUI(source);
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ToUI(const std::string& name, const TModel* source)
{
	_uiComponents->at(name)->ToUI(source);
}
template<class TModel, IsUIBase<TModel> T>
int ScrollViewerUI<TModel, T>::GetUICount() const
{
	return _uiComponents->size();
}
template<class TModel, IsUIBase<TModel> T>
std::string ScrollViewerUI<TModel, T>::GetName(int index) const
{
	auto iter = _uiModels->begin();
	std::advance(iter, index);
	return iter->first;
}
template<class TModel, IsUIBase<TModel> T>
T* ScrollViewerUI<TModel, T>::GetUI(const std::string& name) const
{
	return _uiComponents->at(name);
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::MoveUp(const std::string& name)
{
	//auto iter = _uiModels->find(name);
	//if (iter == _uiModels->begin())
	//	return;

	//// std::advance (previous position)
	//auto iterLast = iter;
	//std::advance(iterLast, -1);

	//int order = iter->second->GetOrder();
	//int orderLast = iterLast->second->GetOrder();

	//// Swap Order #'s
	//iter->second->SetOrder(orderLast);
	//iterLast->second->SetOrder(order);

	//_uiModelOrders->erase(order);
	//_uiModelOrders->erase(orderLast);

	//_uiModelOrders->insert(std::make_pair(iter->second->GetOrder(), iter->second));
	//_uiModelOrders->insert(std::make_pair(iterLast->second->GetOrder(), iterLast->second));

	//this->RebuildUI();
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::MoveDown(const std::string& name)
{
	//auto iter = _uiModels->find(name);
	//if (iter == _uiModels->end())
	//	return;

	//// std::advance (next position)
	//auto iterNext = iter;
	//std::advance(iterNext, 1);

	//int order = iter->second->GetOrder();
	//int orderNext = iterNext->second->GetOrder();

	//// Swap Order #'s
	//iter->second->SetOrder(orderNext);
	//iterNext->second->SetOrder(order);

	//_uiModelOrders->erase(order);
	//_uiModelOrders->erase(orderNext);

	//_uiModelOrders->insert(std::make_pair(iter->second->GetOrder(), iter->second));
	//_uiModelOrders->insert(std::make_pair(iterNext->second->GetOrder(), iterNext->second));

	//this->RebuildUI();
}
template<class TModel, IsUIBase<TModel> T>
bool ScrollViewerUI<TModel, T>::GetDirty() const
{
	bool isDirty = false;

	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end() && !isDirty; ++iter)
	{
		isDirty |= iter->second->GetDirty();
	}

	return isDirty;
}
template<class TModel, IsUIBase<TModel> T>
bool ScrollViewerUI<TModel, T>::GetDirty(const std::string& name) const
{
	return _uiComponents->at(name)->GetDirty();
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ClearDirty()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		iter->second->ClearDirty();
	}
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ClearDirty(const std::string& name)
{
	_uiComponents->at(name)->ClearDirty();
}
template<class TModel, IsUIBase<TModel> T>
bool ScrollViewerUI<TModel, T>::HasPendingAction(const std::string& name) const
{
	return _uiComponents->at(name)->HasPendingAction();
}
template<class TModel, IsUIBase<TModel> T>
bool ScrollViewerUI<TModel, T>::HasPendingAction() const
{
	bool hasPendingAction = false;

	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end() && !hasPendingAction; ++iter)
	{
		hasPendingAction |= iter->second->HasPendingAction();
	}

	return hasPendingAction;
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ClearPendingAction(const std::string& name)
{
	_uiComponents->at(name)->ClearPendingAction();
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::ClearPendingAction()
{
	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		iter->second->ClearPendingAction();
	}
}
template<class TModel, IsUIBase<TModel> T>
void ScrollViewerUI<TModel, T>::RebuildUI()
{
	//if (_uiModels->size() == 0)
	//{
	//	_listComponent->DetachAllChildren();
	//	return;
	//}

	//// Collect Order #'s
	//std::vector<int> orderNumbers;

	//for (auto iter = _uiModels->begin(); iter != _uiModels->end(); ++iter)
	//{
	//	orderNumbers.push_back(iter->second->GetOrder());
	//}

	//std::sort(orderNumbers.begin(), orderNumbers.end(), [](int order1, int order2) { return order1 > order2; });

	//// Rebuild List
	//_listComponent->DetachAllChildren();

	//for (int index = 0; index < orderNumbers.size(); index++)
	//{
	//	// Get name for this (unique) order number
	//	std::string name = _uiModelOrders->at(orderNumbers[index])->GetName();

	//	// Add Component
	//	_listComponent->Add(_uiComponents->at(name)->GetComponent());
	//}

	// Rebuild List
	_listComponent->DetachAllChildren();

	for (auto iter = _uiComponents->begin(); iter != _uiComponents->end(); ++iter)
	{
		// Add Component
		_listComponent->Add(iter->second->GetComponent());
	}
}
#endif