- parametric equation using zc
  - cartesian / polar toggle switch
  - give two function names that are already defined standalone
  - running variable
    - give start and end
    - the step is in "continuous" mode by default: automatically computed
      - Can be un-toggled to be given manually, in which case the curve can be a scatter plot.
- grid calculator: rework usage TARGET_TICKS_NUM (is it actually relevant ?)
- Implement properly reset to default view in settings window for both the linear scale and log and for both axes,
basing off the base given by the user

Exporting
- Make the actual exporting with the user given formats, the format left are image formats
- Implement tick spacing auto-update in export preview: easier in the new approach (simply update the base power)
- Enable adding additional pins in data tables to represent more than one curve per data table

Other work:
- update pictures in appdata/screenshots with new window layouts

Possible improvements:
- Use line-style chooser to offer the ability to change line styles for the grid and subgrid.

Potentially useful QML classes
- Frame
- SwipeView
