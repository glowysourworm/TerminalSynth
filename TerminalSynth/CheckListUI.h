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
#include <string>
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

private:

	ftxui::Component _component;
	ftxui::Component _list;

	float* _scrollY;
	float _scrollDeltaY;

	std::vector<CheckboxUI*>* _checkBoxes;
};

CheckListUI::CheckListUI(const ftxui::Color& labelColor) : UIBase("", "", labelColor)
{
	_checkBoxes = new std::vector<CheckboxUI*>();
	_scrollY = new float(0);
	_scrollDeltaY = 0.005f;
}

CheckListUI::~CheckListUI()
{
	UIBase::~UIBase();

	for (int index = 0; index < _checkBoxes->size(); index++)
	{
		delete _checkBoxes->at(index);
	}

	delete _checkBoxes;
	delete _scrollY;
}

void CheckListUI::Initialize(const std::vector<std::string>& initialValue)
{
	UIBase::Initialize(initialValue);

	_list = ftxui::Container::Vertical({});

	// Store the value in our UI component
	for (int index = 0; index < initialValue.size(); index++)
	{
		// The label conversion here could be strict (something like to_string compatible)
		auto checkboxUI = new CheckboxUI(initialValue[index], initialValue[index], false, ftxui::Color::White);

		checkboxUI->Initialize(false);

		_checkBoxes->push_back(checkboxUI);
		_list->Add(checkboxUI->GetComponent());
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

	_component = ftxui::Container::Horizontal({

		// Must render the list to add the focused relative position offset
		ftxui::Renderer(_list, [&] {
			return _list->Render() | ftxui::focusPositionRelative(0, *_scrollY) | ftxui::frame;
		}),

		scrollBarY

		}) | ftxui::CatchEvent([&](ftxui::Event event) {

			if (event.mouse().button == ftxui::Mouse::Button::WheelUp && _component->Focused())
			{
				// Set Mouse Wheel (clipped [0,1])
				*_scrollY = fminf(fmaxf(*_scrollY - _scrollDeltaY, 0), 1);
			}
			else if (event.mouse().button == ftxui::Mouse::Button::WheelDown && _component->Focused())
			{
				// Set Mouse Wheel (clipped [0,1])
				*_scrollY = fminf(fmaxf(*_scrollY + _scrollDeltaY, 0), 1);
			}

			// Pass through
			if (event.is_mouse())
				return false;

		// Cancel keyboard events
		return true;
	});
}

ftxui::Component CheckListUI::GetComponent()
{
	return _component;
}

void CheckListUI::UpdateComponent(bool clearDirty)
{
	if (clearDirty)
	{
		for (int index = 0; index < _checkBoxes->size(); index++)
		{
			_checkBoxes->at(index)->UpdateComponent(clearDirty);
		}
	}
}

bool CheckListUI::GetDirty() const
{
	bool dirty = false;

	for (int index = 0; index < _checkBoxes->size() && !dirty; index++)
	{
		dirty |= _checkBoxes->at(index)->GetDirty();
	}

	return dirty;
}

void CheckListUI::ToUI(const std::vector<std::string>& entireList)
{

}

void CheckListUI::FromUI(std::vector<std::string>& selectedList, bool clearDirty)
{
	for (int index = 0; index < _checkBoxes->size(); index++)
	{
		bool checked = false;
		_checkBoxes->at(index)->FromUI(checked, clearDirty);

		if (checked)
			selectedList.push_back(_checkBoxes->at(index)->GetName());
	}

	if (clearDirty)
		this->ClearDirty();
}

#endif