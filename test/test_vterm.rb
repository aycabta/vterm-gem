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

  def test_screen
    vterm = VTerm.new(1, 20)
    vterm.set_utf8(true)

    screen = vterm.screen
    assert_kind_of(VTerm::Screen, screen)
  end

  def test_screen_cell_char
    vterm = VTerm.new(1, 20)
    vterm.set_utf8(true)

    screen = vterm.screen
    screen.reset(true)

    vterm.write('Hello')
    rows, cols = vterm.size
    expected = ['H', 'e', 'l', 'l', 'o', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '']
    rows.times do |r|
      cols.times do |c|
        cell = screen.cell_at(r, c)
        expected_char = expected.shift
        assert_equal(expected_char, cell.char, %Q{(#{r}, #{c}) should be "#{expected_char}", but "#{cell.char}"})
      end
    end
  end
end
