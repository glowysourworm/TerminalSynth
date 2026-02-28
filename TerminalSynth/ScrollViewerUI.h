#pragma once

#ifndef SCROLL_VIEWER_UI_H
#define SCROLL_VIEWER_UI_H

#include "UIBase.h"
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
#include <string>
#include <vector>

/// <summary>
/// Concept that ensures that the constraint on the component class is to inherit
/// from UIBase*
/// </summary>
template<typename T, typename TValue>
concept IsUIBase = std::convertible_to<T*, UIBase<TValue>*>;

/// <summary>
/// Container class for UIBase components. The template constraint is for the class T, and
/// class template value TValue, to be properly inherited from UIBase component(s).
/// </summary>
/// <typeparam name="TValue">Model value type name</typeparam>
/// <typeparam name="T">UIBase* class (must inherit from UIBase)</typeparam>
template<typename TValue, IsUIBase<TValue> T>
class ScrollViewerUI
{
public:

	ScrollViewerUI(const std::string& title, const ftxui::Color& titleColor);
	~ScrollViewerUI();

	ftxui::Component GetComponent();

	/// <summary>
	/// Creates a new UI instance and applies the model to it using the default contructor. Also,
	/// calls initialize on the UIBase* instance. If there is an instance (based on == equality)
	/// already in the ScrollViewerUI, then there will be an exception thrown.
	/// </summary>
	void AddUI(const TValue& model);

	/// <summary>
	/// Removes current UI instance based on model == equality. If there is no model in the 
	/// ScrollViewerUI, then an exception is thrown.
	/// </summary>
	void RemoveUI(const TValue& model);
	
	// UIBase Functions:  There could be another component type which is a container for 
	//					  more UIBase components.
	void UpdateComponent();

	void FromUI(int index, TValue& destination);
	void ToUI(int index, const TValue& source);
	int GetUICount() const;

	bool GetDirty() const;
	void ClearDirty();

private:

	ftxui::Component _component;
	ftxui::Component _listComponent;

	std::string* _title;
	ftxui::Color* _titleColor;

	bool* _isMouseOver;
	float* _scrollY;
	float _scrollDeltaY;

	std::vector<T*>* _uiComponents;			// T : UIBase
	std::vector<TValue*>* _uiModels;		// TValue (must have copy constructor!)
};

template<typename TValue, IsUIBase<TValue> T>
ScrollViewerUI<TValue, T>::ScrollViewerUI(const std::string& title, const ftxui::Color& titleColor)
{
	_scrollY = new float(0);
	_scrollDeltaY = 0.05f;
	_isMouseOver = new bool(false);
	_uiComponents = new std::vector<T*>();
	_uiModels = new std::vector<TValue*>();

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

template<typename TValue, IsUIBase<TValue> T>
ScrollViewerUI<TValue, T>::~ScrollViewerUI()
{
	for (int index = 0; index < _uiComponents->size(); index++)
	{
		delete _uiComponents->at(index);
	}
	for (int index = 0; index < _uiModels->size(); index++)
	{
		delete _uiModels->at(index);
	}

	delete _title;
	delete _titleColor;
	delete _isMouseOver;
	delete _scrollY;
	delete _uiComponents;
	delete _uiModels;
}

template<typename TValue, IsUIBase<TValue> T>
ftxui::Component ScrollViewerUI<TValue, T>::GetComponent()
{
	return ftxui::Renderer(_component, [&] { 
		return _component->Render(); 
	});
}

template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::AddUI(const TValue& model)
{
	// Existing Model
	for (int index = 0; index < _uiModels->size(); index++)
	{
		if (model == *_uiModels->at(index))
			throw new std::exception("Existing model matched equality check! Please be sure not to add duplicate model values!");
	}

	// (MEMORY!) Create TValue* model using copy constructor
	TValue* uiModel = new TValue(model);

	// (MEMORY!) Create UIBase* component
	T* uiComponent = new T(model);

	// -> UIBase::Initialize
	uiComponent->Initialize(model);

	// Keep these in the ui tree
	_uiComponents->push_back(uiComponent);
	_uiModels->push_back(uiModel);
	_listComponent->Add(uiComponent->GetComponent());
}
template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::RemoveUI(const TValue& model)
{
	bool existingModel = false;
	int existingModelIndex = -1;

	// Existing Model?
	for (int index = 0; index < _uiModels->size() && !existingModel; index++)
	{
		if (model == *_uiModels->at(index))
		{
			existingModel = true;
			existingModelIndex = index;
		}
	}

	if (!existingModel)
		throw new std::exception("Existing model not found via equality check! Please be sure that model has been added!");

	// MEMORY! (~ScrollViewerUI)
	delete _uiComponents->at(existingModelIndex);
	delete _uiModels->at(existingModelIndex);

	// Remove
	_uiComponents->erase(_uiComponents->begin() + existingModelIndex);
	_uiModels->erase(_uiModels->begin() + existingModelIndex);
	
	// Rebuild List
	_listComponent->DetachAllChildren();

	for (int index = 0; index < _uiComponents->size(); index++)
	{
		_listComponent->Add(_uiComponents->at(index)->GetComponent());
	}
}
template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::UpdateComponent()
{
	for (int index = 0; index < _uiComponents->size(); index++)
	{
		_uiComponents->at(index)->UpdateComponent();
	}
}
template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::FromUI(int index, TValue& destination)
{
	_uiComponents->at(index)->FromUI(destination);
}
template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::ToUI(int index, const TValue& source)
{
	_uiComponents->at(index)->ToUI(source);
}
template<typename TValue, IsUIBase<TValue> T>
int ScrollViewerUI<TValue, T>::GetUICount() const
{
	return _uiComponents->size();
}
template<typename TValue, IsUIBase<TValue> T>
bool ScrollViewerUI<TValue, T>::GetDirty() const
{
	bool isDirty = false;
	
	for (int index = 0; index < _uiComponents->size() && !isDirty; index++)
	{
		isDirty |= _uiComponents->at(index)->GetDirty();
	}

	return isDirty;
}
template<typename TValue, IsUIBase<TValue> T>
void ScrollViewerUI<TValue, T>::ClearDirty()
{
	for (int index = 0; index < _uiComponents->size(); index++)
	{
		_uiComponents->at(index)->ClearDirty();
	}
}
#endif