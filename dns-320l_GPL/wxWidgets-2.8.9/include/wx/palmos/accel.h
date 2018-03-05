/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: accel.h,v 1.1.1.1 2009/10/09 02:57:21 jack Exp $
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

// ----------------------------------------------------------------------------
// the accel table has all accelerators for a given window or menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorTable : public wxObject
{
public:
    // default ctor
    wxAcceleratorTable();

    // load from .rc resource (Windows specific)
    wxAcceleratorTable(const wxString& resource);

    // initialize from array
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    virtual ~wxAcceleratorTable();

    bool Ok() const { return IsOk(); }
    bool IsOk() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL() const;

    // translate the accelerator, return true if done
    bool Translate(wxWindow *window, WXMSG *msg) const;

private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif
    // _WX_ACCEL_H_