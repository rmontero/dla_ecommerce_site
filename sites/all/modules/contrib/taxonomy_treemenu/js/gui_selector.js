// $id$

function updateTargetBranch(idStr) {
  var ids = idStr.split(":");
  document.getElementById('branch-selector-display-vid').innerHTML = ids[0];
  document.getElementById('branch-selector-display-tid').innerHTML = ids[1];
  document.getElementById('edit-tree-root-display-tree-root-data').value = idStr;
}
