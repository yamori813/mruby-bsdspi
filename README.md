# mruby-bsdspi   [![Build Status](https://travis-ci.org/yamori813/mruby-bsdspi.svg?branch=master)](https://travis-ci.org/yamori813/mruby-bsdspi)
BsdSpi class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'yamori813/mruby-bsdspi'
end
```
## example
```ruby
p BsdSpi.hi
#=> "hi!!"
t = BsdSpi.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the BSD License:
- see LICENSE file
