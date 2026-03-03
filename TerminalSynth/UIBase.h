#pragma once

#ifndef UIBASE_H
#define UIBASE_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/node.hpp>

/// <summary>
/// Class container for ftxui components. Provides functions for both Element
/// and Component rendering situations.
/// </summary>
template<typename T>
class UIBase
{
public:

	UIBase();
	UIBase(const T& initialValue) {};
	~UIBase();

	/// <summary>
	/// Initializes UI component from initial data
	/// </summary>
	/// <param name="initialValue">Initial value of the data for display</param>
	virtual void Initialize(const T& initialValue) = 0;

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
	/// Sets the UI from the (current) value. This will be called in real time.
	/// </summary>
	virtual void ToUI(const T* source) = 0;

	/// <summary>
	/// Sets the destination parameter from the UI. This will be called when the
	/// UI's dirty flag is set.
	/// </summary>
	virtual void FromUI(T& destination) = 0;

	/// <summary>
	/// Sets the destination parameter from the UI. This will be called when the
	/// UI's dirty flag is set.
	/// </summary>
	virtual void FromUI(T* destination) = 0;

	/// <summary>
	/// Process one cycle of the UI. This will allow capture of action flags between UI components
	/// that need to process before UpdateComponent / FromUI are called. 
	/// </summary>
	virtual void Tick() = 0;

	/// <summary>
	/// Services pending action for the UI. This must be run when the HasPendingAction returns true.
	/// You can choose how to utilize the pending action flag; but it must be servicable from the
	/// top-down controller method.
	/// </summary>
	virtual void ServicePendingAction() = 0;

	/// <summary>
	/// Returns status flag for pending UI actions. This must be cleared before UpdateComponent() 
	/// is called. 
	/// </summary>
	virtual bool HasPendingAction() const = 0;

	/// <summary>
	/// Clears pending action flag for the component. This should be called by the direct parent
	/// component.
	/// </summary>
	virtual void ClearPendingAction() = 0;

	/// <summary>
	/// Forces component to update itself, using action flags and dirty status to setup the
	/// proper UI state for drawing (which uses FTXUI).
	/// </summary>
	virtual void UpdateComponent() = 0;

	/// <summary>
	/// Returns status of the UI (has there been changes)
	/// </summary>
	virtual bool GetDirty() const = 0;

	/// <summary>
	/// Clears the dirty status of the UI
	/// </summary>
	virtual void ClearDirty() = 0;

private:

	ftxui::Component _componentUI;
};

template<typename T>
UIBase<T>::UIBase()
{
	_componentUI = nullptr;
}

template<typename T>
UIBase<T>::~UIBase()
{
}

template<typename T>
ftxui::Element UIBase<T>::Render()
{
	this->UpdateComponent(false);

	if (_componentUI == nullptr)
		_componentUI = this->GetComponent();

	return _componentUI->Render();
}

template<typename T>
ftxui::Component UIBase<T>::GetRenderer()
{
	if (_componentUI == nullptr)
		_componentUI = this->GetComponent();

	return ftxui::Renderer(_componentUI, [&] {

		return _componentUI->Render();
	});
}

#endif
