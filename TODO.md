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

Low priority:
- Do not drop sampled data when start/end/step are invalid, because the data may still be useful when they get fixed
