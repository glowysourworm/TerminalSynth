#pragma once

#ifndef CHECK_LIST_UI_H
#define CHECK_LIST_UI_H

#include "CheckboxUI.h"
#include "UIBase.h"
#include <cmath>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

class CheckListUI : public UIBase<std::vector<std::string>>
{
public:

	CheckListUI(const ftxui::Color& labelColor);
	~CheckListUI();

	void Initialize(const std::vector<std::string>& initialValue) override;
	ftxui::Component GetComponent() override;
	void UpdateComponent(bool clearDirty) override;

	void ToUI(const std::vector<std::string>& entireList) override;
	void FromUI(std::vector<std::string>& selectedList, bool clearDirty) override;

	bool GetDirty() const override;

	bool IsSelected(const std::string& name) const;

private:

	ftxui::Component _component;
	ftxui::Component _listComponent;

	bool* _isMouseOver;
	float* _scrollY;
	float _scrollDeltaY;

	std::map<std::string, CheckboxUI*>* _checkBoxes;
};

CheckListUI::CheckListUI(const ftxui::Color& labelColor) : UIBase("", "", labelColor)
{
	_checkBoxes = new std::map<std::string, CheckboxUI*>();
	_scrollY = new float(0);
	_scrollDeltaY = 0.005f;
	_isMouseOver = new bool(false);
}

CheckListUI::~CheckListUI()
{
	UIBase::~UIBase();

	for (auto iter = _checkBoxes->begin(); iter != _checkBoxes->end(); ++iter)
	{
		delete iter->second;
	}

	delete _checkBoxes;
	delete _scrollY;
	delete _isMouseOver;
}

void CheckListUI::Initialize(const std::vector<std::string>& initialValue)
{
	UIBase::Initialize(initialValue);

	_listComponent = ftxui::Container::Vertical({});

	// Store the value in our UI component
	for (int index = 0; index < initialValue.size(); index++)
	{
		// The label conversion here could be strict (something like to_string compatible)
		auto checkboxUI = new CheckboxUI(initialValue[index], initialValue[index], false, ftxui::Color::White);

		checkboxUI->Initialize(false);

		_checkBoxes->insert(std::make_pair(initialValue[index], checkboxUI));
		_listComponent->Add(checkboxUI->GetComponent());
	}

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

	_component = ftxui::Container::Vertical(
	{
		ftxui::Renderer([&] { return ftxui::text("Airwindows (airwindows@github.com)"); }),
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

ftxui::Component CheckListUI::GetComponent()
{
	return _component;
}

void CheckListUI::UpdateComponent(bool clearDirty)
{
	if (clearDirty)
	{
		for (auto iter = _checkBoxes->begin(); iter != _checkBoxes->end(); ++iter)
		{
			iter->second->UpdateComponent(clearDirty);
		}
	}
}

bool CheckListUI::GetDirty() const
{
	bool dirty = false;

	for (auto iter = _checkBoxes->begin(); iter != _checkBoxes->end(); ++iter)
	{
		dirty |= iter->second->GetDirty();
	}

	return dirty;
}

inline bool CheckListUI::IsSelected(const std::string& name) const
{
	bool result = false;
	_checkBoxes->at(name)->FromUI(result, false);
	return result;
}

void CheckListUI::ToUI(const std::vector<std::string>& entireList)
{

}

void CheckListUI::FromUI(std::vector<std::string>& selectedList, bool clearDirty)
{
	for (auto iter = _checkBoxes->begin(); iter != _checkBoxes->end(); ++iter)
	{
		bool checked = false;
		iter->second->FromUI(checked, clearDirty);

		if (checked)
			selectedList.push_back(iter->first);
	}

	if (clearDirty)
		this->ClearDirty();
}

#endif