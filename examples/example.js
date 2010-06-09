var webgl = require('../webgl'),
    example = require('./example-include');

var window = new webgl.Window(640, 480);
example.init(window, true);
