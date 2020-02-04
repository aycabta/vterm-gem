$LOAD_PATH.unshift File.expand_path('../lib', __FILE__)
require 'test/unit'
require 'vterm'

class VTerm::Test < Test::Unit::TestCase
  def setup
  end

  def teardown
  end

  def test_size
    vterm = VTerm.new(1, 20)
    vterm.set_utf8(true)

    assert_equal([1, 20], vterm.size)
  end
end
