
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __javax_swing_text_BoxView__
#define __javax_swing_text_BoxView__

#pragma interface

#include <javax/swing/text/CompositeView.h>
#include <gcj/array.h>

extern "Java"
{
  namespace java
  {
    namespace awt
    {
        class Graphics;
        class Rectangle;
        class Shape;
    }
  }
  namespace javax
  {
    namespace swing
    {
        class SizeRequirements;
      namespace event
      {
          class DocumentEvent;
          class DocumentEvent$ElementChange;
      }
      namespace text
      {
          class BoxView;
          class Element;
          class Position$Bias;
          class View;
          class ViewFactory;
      }
    }
  }
}

class javax::swing::text::BoxView : public ::javax::swing::text::CompositeView
{

public:
  BoxView(::javax::swing::text::Element *, jint);
  virtual jint getAxis();
  virtual void setAxis(jint);
  virtual void layoutChanged(jint);
public: // actually protected
  virtual jboolean isLayoutValid(jint);
  virtual void paintChild(::java::awt::Graphics *, ::java::awt::Rectangle *, jint);
public:
  virtual void replace(jint, jint, JArray< ::javax::swing::text::View * > *);
private:
  JArray< jint > * replaceLayoutArray(JArray< jint > *, jint, jint);
public:
  virtual void paint(::java::awt::Graphics *, ::java::awt::Shape *);
  virtual jfloat getPreferredSpan(jint);
  virtual jfloat getMaximumSpan(jint);
  virtual jfloat getMinimumSpan(jint);
public: // actually protected
  virtual ::javax::swing::SizeRequirements * baselineRequirements(jint, ::javax::swing::SizeRequirements *);
  virtual void baselineLayout(jint, jint, JArray< jint > *, JArray< jint > *);
  virtual ::javax::swing::SizeRequirements * calculateMajorAxisRequirements(jint, ::javax::swing::SizeRequirements *);
  virtual ::javax::swing::SizeRequirements * calculateMinorAxisRequirements(jint, ::javax::swing::SizeRequirements *);
  virtual jboolean isBefore(jint, jint, ::java::awt::Rectangle *);
  virtual jboolean isAfter(jint, jint, ::java::awt::Rectangle *);
  virtual ::javax::swing::text::View * getViewAtPoint(jint, jint, ::java::awt::Rectangle *);
  virtual void childAllocation(jint, ::java::awt::Rectangle *);
  virtual void layout(jint, jint);
private:
  void layoutAxis(jint, jint);
public: // actually protected
  virtual void layoutMajorAxis(jint, jint, JArray< jint > *, JArray< jint > *);
  virtual void layoutMinorAxis(jint, jint, JArray< jint > *, JArray< jint > *);
  virtual jboolean isAllocationValid();
public:
  virtual jint getWidth();
  virtual jint getHeight();
  virtual void setSize(jfloat, jfloat);
public: // actually protected
  virtual jint getSpan(jint, jint);
  virtual jint getOffset(jint, jint);
public:
  virtual jfloat getAlignment(jint);
  virtual void preferenceChanged(::javax::swing::text::View *, jboolean, jboolean);
  virtual ::java::awt::Shape * modelToView(jint, ::java::awt::Shape *, ::javax::swing::text::Position$Bias *);
  virtual jint getResizeWeight(jint);
  virtual ::java::awt::Shape * getChildAllocation(jint, ::java::awt::Shape *);
public: // actually protected
  virtual void forwardUpdate(::javax::swing::event::DocumentEvent$ElementChange *, ::javax::swing::event::DocumentEvent *, ::java::awt::Shape *, ::javax::swing::text::ViewFactory *);
public:
  virtual jint viewToModel(jfloat, jfloat, ::java::awt::Shape *, JArray< ::javax::swing::text::Position$Bias * > *);
public: // actually protected
  virtual jboolean flipEastAndWestAtEnds(jint, ::javax::swing::text::Position$Bias *);
private:
  void updateRequirements(jint);
  jint __attribute__((aligned(__alignof__( ::javax::swing::text::CompositeView)))) myAxis;
  JArray< jboolean > * layoutValid;
  JArray< jboolean > * requirementsValid;
  JArray< JArray< jint > * > * spans;
  JArray< JArray< jint > * > * offsets;
  JArray< ::javax::swing::SizeRequirements * > * requirements;
  JArray< jint > * span;
  ::java::awt::Rectangle * tmpRect;
  ::java::awt::Rectangle * clipRect;
public:
  static ::java::lang::Class class$;
};

#endif // __javax_swing_text_BoxView__