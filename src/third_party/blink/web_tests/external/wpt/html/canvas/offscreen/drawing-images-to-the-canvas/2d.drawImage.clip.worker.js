// META: timeout=long
// DO NOT EDIT! This test has been generated by /html/canvas/tools/gentest.py.
// OffscreenCanvas test in a worker:2d.drawImage.clip
// Description:
// Note:

importScripts("/resources/testharness.js");
importScripts("/html/canvas/resources/canvas-tests.js");

var t = async_test("");
var t_pass = t.done.bind(t);
var t_fail = t.step_func(function(reason) {
    throw reason;
});
t.step(function() {

var canvas = new OffscreenCanvas(100, 50);
var ctx = canvas.getContext('2d');

ctx.fillStyle = '#0f0';
ctx.fillRect(0, 0, 100, 50);
ctx.rect(-10, -10, 1, 1);
ctx.clip();
fetch('/images/red.png')
  .then(response => response.blob())
    .then(blob => {
      createImageBitmap(blob)
        .then(bitmap => {
        ctx.fillStyle = '#0f0';
        ctx.fillRect(0, 0, 100, 50);
        ctx.rect(-10, -10, 1, 1);
        ctx.clip();
        ctx.drawImage(document.getElementById('red.png'), 0, 0);
        _assertPixelApprox(canvas, 50,25, 0,255,0,255, 2);
    });
});
t.done();

});
done();
