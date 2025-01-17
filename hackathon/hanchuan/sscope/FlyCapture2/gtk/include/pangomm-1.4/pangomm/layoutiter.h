// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _PANGOMM_LAYOUTITER_H
#define _PANGOMM_LAYOUTITER_H


#include <glibmm.h>

/* $Id: layoutiter.hg,v 1.2 2003/12/14 11:54:05 murrayc Exp $ */

/* layoutiter.h
 *
 * Copyright 2001-2002 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <pangomm/layoutline.h>
#include <pangomm/layoutrun.h>
#include <pango/pango-layout.h>


namespace Pango
{

/** A Pango::LayoutIter can be used to iterate over the visual extents of a Pango::Layout.
 */
class LayoutIter
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef LayoutIter CppObjectType;
  typedef PangoLayoutIter BaseObjectType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:

  
public:
  // There's no other ctor, and the default ctor creates an invalid object.
  // Therefore, Pango::LayoutIter is usable only as output argument.
  LayoutIter();
  ~LayoutIter();

  
  /** Gets the current byte index. Note that iterating forward by char
   * moves in visual order, not logical order, so indexes may not be
   * sequential. Also, the index may be equal to the length of the text
   * in the layout, if on the <tt>0</tt> run (see pango_layout_iter_get_run()).
   * @return Current byte index.
   */
  int get_index() const;
  
  /** Gets the current run. When iterating by run, at the end of each
   * line, there's a position with a <tt>0</tt> run, so this function can return
   * <tt>0</tt>. The <tt>0</tt> run at the end of each line ensures that all lines have
   * at least one run, even lines consisting of only a newline.
   * 
   * Use the faster pango_layout_iter_get_run_readonly() if you do not plan
   * to modify the contents of the run (glyphs, glyph widths, etc.).
   * @return The current run.
   */
  LayoutRun get_run() const;
  //TODO: Use pango_layout_iter_get_run_readonly()?

  
  /** Gets the current line.
   * 
   * Use the faster pango_layout_iter_get_line_readonly() if you do not plan
   * to modify the contents of the line (glyphs, glyph widths, etc.).
   * @return The current line.
   */
  Glib::RefPtr<LayoutLine> get_line() const;
  //TODO: We should really have a const and non-const version: _WRAP_METHOD(Glib::RefPtr<const LayoutLine> get_line() const, pango_layout_iter_get_line_readonly, refreturn)


  /** Determines whether @a iter is on the last line of the layout.
   * @return <tt>true</tt> if @a iter is on the last line.
   */
  bool at_last_line() const;

  
  /** Gets the layout associated with a Pango::LayoutIter.
   * @return The layout associated with @a iter.
   * 
   * Since: 1.20.
   */
  Glib::RefPtr<Layout> get_layout();
  
  /** Gets the layout associated with a Pango::LayoutIter.
   * @return The layout associated with @a iter.
   * 
   * Since: 1.20.
   */
  Glib::RefPtr<const Layout> get_layout() const;

  
  /** Moves @a iter forward to the next character in visual order. If @a iter was already at
   * the end of the layout, returns <tt>false</tt>.
   * @return Whether motion was possible.
   */
  bool next_char();
  
  /** Moves @a iter forward to the next cluster in visual order. If @a iter
   * was already at the end of the layout, returns <tt>false</tt>.
   * @return Whether motion was possible.
   */
  bool next_cluster();
  
  /** Moves @a iter forward to the next run in visual order. If @a iter was
   * already at the end of the layout, returns <tt>false</tt>.
   * @return Whether motion was possible.
   */
  bool next_run();
  
  /** Moves @a iter forward to the start of the next line. If @a iter is
   * already on the last line, returns <tt>false</tt>.
   * @return Whether motion was possible.
   */
  bool next_line();

  /** Gets the extents of the current character, in layout coordinates (origin is the top left of the entire layout).
   * Only logical extents can sensibly be obtained for characters; ink extents make sense only down to the level of clusters.
   * @return The logical extents of the current character.
   */
  Rectangle get_char_extents() const;

  
  /** Gets the extents of the current cluster, in layout coordinates
   * (origin is the top left of the entire layout).
   * @param ink_rect Rectangle to fill with ink extents, or <tt>0</tt>.
   * @param logical_rect Rectangle to fill with logical extents, or <tt>0</tt>.
   */
  void get_cluster_extents(Rectangle& ink_rect, Rectangle& logical_rect) const;

  /** Gets the ink extents of the current cluster, in layout coordinates (origin is the top left of the entire layout).
   * @return The extents of the current cluster as drawn.
   */
  Rectangle get_cluster_ink_extents() const;

  /** Gets the logical extents of the current cluster, in layout coordinates (origin is the top left of the entire layout).
   * @return The logical extents of the current cluster.
   */
  Rectangle get_cluster_logical_extents() const;

  
  /** Gets the extents of the current run in layout coordinates
   * (origin is the top left of the entire layout).
   * @param ink_rect Rectangle to fill with ink extents, or <tt>0</tt>.
   * @param logical_rect Rectangle to fill with logical extents, or <tt>0</tt>.
   */
  void get_run_extents(Rectangle& ink_rect, Rectangle& logical_rect) const;

  /** Gets the ink extents of the current run in layout coordinates (origin is the top left of the entire layout).
   * @return The extents of the current run as drawn.
   */
  Rectangle get_run_ink_extents() const;

  /** Gets the logical extents of the current run in layout coordinates (origin is the top left of the entire layout).
   * @return The logical extents of the current run.
   */
  Rectangle get_run_logical_extents() const;

  
  /** Obtains the extents of the current line. @a ink_rect or @a logical_rect
   * can be <tt>0</tt> if you aren't interested in them. Extents are in layout
   * coordinates (origin is the top-left corner of the entire
   * Pango::Layout).  Thus the extents returned by this function will be
   * the same width/height but not at the same x/y as the extents
   * returned from pango_layout_line_get_extents().
   * @param ink_rect Rectangle to fill with ink extents, or <tt>0</tt>.
   * @param logical_rect Rectangle to fill with logical extents, or <tt>0</tt>.
   */
  void get_line_extents(Rectangle& ink_rect, Rectangle& logical_rect) const;

  /** Obtains the ink extents of the current line.
   * @return The extents of the current line as drawn.
   */
  Rectangle get_line_ink_extents() const;

  /** Obtains the logical extents of the current line.
   * @return The logical extents of the current line.
   */
  Rectangle get_line_logical_extents() const;

  
  /** Divides the vertical space in the Pango::Layout being iterated over
   * between the lines in the layout, and returns the space belonging to
   * the current line.  A line's range includes the line's logical
   * extents, plus half of the spacing above and below the line, if
   * pango_layout_set_spacing() has been called to set layout spacing.
   * The Y positions are in layout coordinates (origin at top left of the
   * entire layout).
   * @param y0 Start of line.
   * @param y1 End of line.
   */
  void get_line_yrange(int& y0, int& y1) const;

  
  /** Obtains the extents of the Pango::Layout being iterated
   * over. @a ink_rect or @a logical_rect can be <tt>0</tt> if you
   * aren't interested in them.
   * @param ink_rect Rectangle to fill with ink extents, or <tt>0</tt>.
   * @param logical_rect Rectangle to fill with logical extents, or <tt>0</tt>.
   */
  void get_layout_extents(Rectangle& ink_rect, Rectangle& logical_rect) const;

  /** Obtains the ink extents of the Pango::Layout being iterated over.
   * @return The extents of the layout as drawn.
   */
  Rectangle get_layout_ink_extents() const;

  /** Obtains the logical extents of the Pango::Layout being iterated over.
   * @return The logical extents of the layout.
   */
  Rectangle get_layout_logical_extents() const;

  
  /** Gets the Y position of the current line's baseline, in layout
   * coordinates (origin at top left of the entire layout).
   * @return Baseline of current line.
   */
  int get_baseline() const;

  /// Provides access to the underlying C GObject.  
  PangoLayoutIter*       gobj()       { return gobject_; }
  /// Provides access to the underlying C GObject.
  const PangoLayoutIter* gobj() const { return gobject_; }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  void assign_gobj(PangoLayoutIter* src);
#endif

protected:
  PangoLayoutIter* gobject_;

private:
  // noncopyable
  LayoutIter(const LayoutIter&);
  LayoutIter& operator=(const LayoutIter&);


};

} //namespace Pango


#endif /* _PANGOMM_LAYOUTITER_H */

