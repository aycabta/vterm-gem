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

  def test_read
    vterm = VTerm.new(1, 20)
    vterm.set_utf8(true)

    screen = vterm.screen
    screen.reset(true)

    vterm.write("\e[6n")
    assert_equal("\e[1;1R", vterm.read)
    vterm.write("aiueo\e[6n")
    assert_equal("\e[1;", vterm.read(4))
    assert_equal("6R", vterm.read(4))
  end

  def test_width
    vterm = VTerm.new(2, 5)
    vterm.set_utf8(true)

    screen = vterm.screen
    screen.reset(true)

    vterm.write("abcdefg")
    result = ''
    rows, cols = vterm.size
    rows.times do |r|
      line = ''
      cols.times do |c|
        cell = screen.cell_at(r, c)
        line << cell.char if cell.char
      end
      line.gsub!(/ *$/, '')
      result << line + "\n"
    end
    assert_equal(<<~EXPECTED, result)
      abcde
      fg
    EXPECTED
  end
end
