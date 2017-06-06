##
## BsdSpi Test
##

assert("BsdSpi#hello") do
  t = BsdSpi.new "hello"
  assert_equal("hello", t.hello)
end

assert("BsdSpi#bye") do
  t = BsdSpi.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("BsdSpi.hi") do
  assert_equal("hi!!", BsdSpi.hi)
end
