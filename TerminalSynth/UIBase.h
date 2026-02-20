#pragma once

#ifndef UIBASE_H
#define UIBASE_H

#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <string>

/// <summary>
/// Class container for ftxui components. Provides functions for both Element
/// and Component rendering situations.
/// </summary>
template<typename T>
class UIBase
{
public:

	UIBase(const std::string& name, const std::string& label, const ftxui::Color& labelColor);
	~UIBase();

	/// <summary>
	/// Initializes UI component from initial data
	/// </summary>
	/// <param name="initialValue">Initial value of the data for display</param>
	virtual void Initialize(const T& initialValue);

	/// <summary>
	/// Creates FTXUI dom element for rendering
	/// </summary>
	ftxui::Element Render();

	/// <summary>
	/// Creaets FTXUI component renderer for use in rendering the underlying UI
	/// </summary>
	ftxui::Component GetRenderer();

	/// <summary>
	/// Gets FTXUI component for rendering
	/// </summary>
	virtual ftxui::Component GetComponent() = 0;

	/// <summary>
	/// Sets the UI from the (current) value. This will be called in real time.
	/// </summary>
	virtual void ToUI(const T& source) = 0;

	/// <summary>
	/// Sets the destination parameter from the UI. This will be called when the
	/// UI's dirty flag is set.
	/// </summary>
	virtual void FromUI(T& destination, bool clearDirty) = 0;

	/// <summary>
	/// Forces component to update itself, recreating any variables that it needs for
	/// the drawing code to render it.
	/// </summary>
	virtual void UpdateComponent(bool clearDirty) = 0;

	/// <summary>
	/// Returns status of the UI (has there been changes)
	/// </summary>
	virtual bool GetDirty() const;

	std::string GetLabel() const { return *_label; }
	std::string GetName() const { return *_name; }
	ftxui::Color GetLabelColor() const { return *_labelColor; }

protected:

	void SetDirty();
	void ClearDirty();

private:

	std::string* _name;
	std::string* _label;	
	ftxui::Color* _labelColor;

	ftxui::Component _componentUI;

	bool _isDirty;
	bool _isInitialized;
};

template<typename T>
UIBase<T>::UIBase(const std::string& name, const std::string& label, const ftxui::Color& labelColor)
{
	_name = new std::string(name);
	_label = new std::string(label);
	_labelColor = new ftxui::Color(labelColor);

	_componentUI = nullptr;
	_isDirty = false;
	_isInitialized = false;
}

template<typename T>
UIBase<T>::~UIBase()
{
	delete _name;
	delete _label;
	delete _labelColor;
}

template<typename T>
void UIBase<T>::Initialize(const T& initialValue)
{
	if (_isInitialized)
		throw new std::exception("UIBase component already initialized!");

	_isInitialized = true;
}

template<typename T>
ftxui::Element UIBase<T>::Render()
{
	if (!_isInitialized)
		throw new std::exception("UIBase component not initialized before using");

	this->UpdateComponent(false);

	if (_componentUI == nullptr)
		_componentUI = this->GetComponent();

	return _componentUI->Render();
}

template<typename T>
ftxui::Component UIBase<T>::GetRenderer()
{
	if (!_isInitialized)
		throw new std::exception("UIBase component not initialized before using");

	if (_componentUI == nullptr)
		_componentUI = this->GetComponent();

	return ftxui::Renderer(_componentUI, [&] {

		return _componentUI->Render();
	});
}

template<typename T>
bool UIBase<T>::GetDirty() const
{
	return _isDirty;
}

template<typename T>
void UIBase<T>::SetDirty()
{
	_isDirty = true;
}

template<typename T>
void UIBase<T>::ClearDirty()
{
	_isDirty = false;
}

#endif
