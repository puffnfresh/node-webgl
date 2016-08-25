all:
  node-waf configure build
  npm install https://github.com/pufuwozu/v8-typed-array/tarball/master
test: all
  node examples/example.js
  chromium-browser examples/example.html
macOS:
  export SDL_VIDEODRIVER=x11
  node-waf configure build
  npm install https://github.com/pufuwozu/v8-typed-array/tarball/master
