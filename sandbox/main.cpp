
#include <ctre.hpp>

#include <iostream>
#include <sstream>
#include <optional>

using namespace std::literals;

enum class type {
  unknown,
  identifier, number, real, boolean, string,
  assignment, lsquare, rsquare, lparen, rparen, greater, lesser, equal, greater_equal, lesser_equal, not_equal, comma, plus, mul, div, minus, dot,
  keyword_if, keyword_end, keyword_while, keyword_or, keyword_not, keyword_and
};

struct location
{
  size_t start{0};
  size_t end{0};
};

struct lex_item {
  type t;
  std::string_view c;
  location loc;
};

class Lexer
{
public:
  Lexer() = default;

  void setInput(std::string_view input)
  {
    _input = input;
    _begin = _input.begin();
    _start = _begin;
    _end = _input.end();
  }

  lex_item lex() {
    static constexpr auto pattern = ctll::fixed_string{ "(\\.)|(\\+)|(\\*)|(/)|(\\-)|(,)|(<>)|(<)|(==)|(>=)|(<=)|(>)|(=)|(\\[)|(\\])|(\\()|(\\))|(if)|(end)|(while)|(or)|(not)|(and)|([a-zA-Z][_a-zA-Z0-9]*)|((?:[1-9][0-9]*|0)\\.[0-9]+(?:[Ee][\\+\\-]?[0-9]+)?)|([0-9]+)|(true|false)|('(?:(?:[^'\\\\]|.)*)')|([ \t]+)|([\n\r]+)|(#[^\n\r]+)" };

    lex_item item;

    while(_start < _end) {
      if (auto [m,dot,plus,mul,div,minus,comma,not_equal,lesser,equal,greater_equal,lesser_equal,greater,assignment,lsquare,rsquare,lparen,rparen,keyword_if,keyword_end,keyword_while,keyword_or,keyword_not,keyword_and,id,real,num,boolean,string,comment,ws,nl] = ctre::search<pattern>(_start, _end); m) {
        if (id) {
          calc_start(_start, id);
          item = lex_item{type::identifier, id, get_location(id)};
        } else if (num) {
          calc_start(_start, num);
          item = lex_item{type::number, num, get_location(num)};
        } else if (boolean) {
          calc_start(_start, boolean);
          item = lex_item{type::boolean, boolean, get_location(boolean)};
        } else if (real) {
          calc_start(_start, real);
          item = lex_item{type::real, real, get_location(real)};
        } else if (string) {
          calc_start(_start, string);
          item = lex_item{type::string, string, get_location(string)};
        } else if (comma) {
          calc_start(_start, comma);
          item = lex_item{type::comma, comma, get_location(comma)};
        } else if (dot) {
          calc_start(_start, dot);
          item = lex_item{type::dot, dot, get_location(dot)};
        } else if (plus) {
          calc_start(_start, plus);
          item = lex_item{type::plus, plus, get_location(plus)};
        } else if (mul) {
          calc_start(_start, mul);
          item = lex_item{type::mul, mul, get_location(mul)};
        } else if (div) {
          calc_start(_start, div);
          item = lex_item{type::div, div, get_location(div)};
        } else if (minus) {
          calc_start(_start, minus);
          item = lex_item{type::minus, minus, get_location(minus)};
        } else if (not_equal) {
          calc_start(_start, not_equal);
          item = lex_item{type::not_equal, not_equal, get_location(not_equal)};
        } else if (lesser) {
          calc_start(_start, lesser);
          item = lex_item{type::lesser, lesser, get_location(lesser)};
        } else if (equal) {
          calc_start(_start, equal);
          item = lex_item{type::equal, equal, get_location(equal)};
        } else if (greater_equal) {
          calc_start(_start, greater_equal);
          item = lex_item{type::greater_equal, greater_equal, get_location(greater_equal)};
        } else if (lesser_equal) {
          calc_start(_start, lesser_equal);
          item = lex_item{type::lesser_equal, lesser_equal, get_location(lesser_equal)};
        } else if (greater) {
          calc_start(_start, greater);
          item = lex_item{type::greater, greater, get_location(greater)};
        } else if (assignment) {
          calc_start(_start, assignment);
          item = lex_item{type::assignment, assignment, get_location(assignment)};
        } else if (lsquare) {
          calc_start(_start, lsquare);
          item = lex_item{type::lsquare, lsquare, get_location(lsquare)};
        } else if (rsquare) {
          calc_start(_start, rsquare);
          item = lex_item{type::rsquare, rsquare, get_location(rsquare)};
        } else if (lparen) {
          calc_start(_start, lparen);
          item = lex_item{type::lparen, lparen, get_location(lparen)};
        } else if (rparen) {
          calc_start(_start, rparen);
          item = lex_item{type::rparen, rparen, get_location(rparen)};
        } else if (keyword_if) {
          calc_start(_start, keyword_if);
          item = lex_item{type::keyword_if, keyword_if, get_location(keyword_if)};
        } else if (keyword_end) {
          calc_start(_start, keyword_end);
          item = lex_item{type::keyword_end, keyword_end, get_location(keyword_end)};
        } else if (keyword_while) {
          calc_start(_start, keyword_while);
          item = lex_item{type::keyword_while, keyword_while, get_location(keyword_while)};
        } else if (keyword_or) {
          calc_start(_start, keyword_or);
          item = lex_item{type::keyword_or, keyword_or, get_location(keyword_or)};
        } else if (keyword_and) {
          calc_start(_start, keyword_and);
          item = lex_item{type::keyword_and, keyword_and, get_location(keyword_and)};
        } else if (keyword_not) {
          calc_start(_start, keyword_not);
          item = lex_item{type::keyword_not, keyword_not, get_location(keyword_not)};
        } else if (comment) {
          calc_start(_start, comment);
        } else if (ws) {
          calc_start(_start, ws);
        } else if (nl) {
          calc_start(_start, nl);
        } else {
          auto loc = get_location(m);
          std::stringstream ss;
          ss << "could not match " << std::string{_start, m.begin()} << " at " << loc.start << ":" << loc.end;
          throw std::runtime_error{ss.str()};
        }
        if (!ws && !nl && !comment) {
          return item;
        }
      }
    }

    return lex_item{};
  }

private:
  template<typename T>
  location get_location(const T& match)
  {
    location loc;
    loc.start = std::distance(_begin, match.begin());
    loc.end = std::distance(_begin, match.end()-1);

    return loc;
  }

  template<typename T>
  void calc_start(std::string_view::const_iterator& start, const T& match)
  {
    check_start(start, match);
    start += std::distance(match.begin(), match.end());
  }

  template<typename T>
  void check_start(std::string_view::const_iterator& start, const T& match)
  {
    if (start != match.begin()) {
      auto loc = get_location(match);
      std::stringstream ss;
      ss << "could not match " << std::string{start, match.begin()} << " at " << loc.start << ":" << loc.end;
      throw std::runtime_error{ss.str()};
    }
  }

  std::string_view _input;
  std::string_view::const_iterator _begin;
  std::string_view::const_iterator _start;
  std::string_view::const_iterator _end;
};


int main(int argc, char** argv)
{
  try {
    Lexer lexer;

    lexer.setInput(R"(
 # Generates the books cover image

 pi = 3.141592653589

 # Camera
 camera = Camera()
 camera.from = (-6, 6, -10)
 camera.to = (6, 0, 6)
 camera.up = [-0.45, 1, 0]
 camera.field_of_view = pi / 3
 camera.horizontal = 1500
 camera.vertical = 1500

 # World and Lights
 world = World()
 world.add(Light(Point(50, 100, -50), Color(1, 1, 1)))
 world.add(Light(Point(-400, 50, -10), Color(0.2, 0.2, 0.2)))

 # Materials
 white_material = Material()
 white_material.color = Color(1, 1, 1)
 white_material.ambient = 0.1
 white_material.diffuse = 0.7
 white_material.specular = 0.0
 white_material.reflective = 0.1

 blue_material = Material(white_material)
 blue_material.color = Color(0.537, 0.831, 0.914)

 red_material = Material(white_material)
 red_material.color = Color(0.941, 0.322, 0.388)

 purple_material = Material(white_material)
 purple_material.color = Color(0.373, 0.404, 0.550)

 # Scene
 backdrop_material = Material()
 backdrop_material.color = Color(1, 1, 1)
 backdrop_material.ambient = 1
 backdrop_material.diffuse = 0
 backdrop_material.specular = 0
 backdrop = Plane(backdrop_material).rotate_x(pi / 2).translate(0, 0, 500)
 world.add(backdrop)

 sphere_material = Material()
 sphere_material.color = Color(0.373, 0.404, 0.550)
 sphere_material.diffuse = 0.2
 sphere_material.ambient = 0.0
 sphere_material.specular = 1.0
 sphere_material.shininess = 200
 sphere_material.reflective = 0.7
 sphere_material.transparency = 0.7
 sphere_material.refractive_index = 1.5
 sphere = Sphere(sphere_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5)
 world.add(sphere)

 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3, 3, 3).translate(4, 0, 0))
 world.add(Cube(blue_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(8.5, 1.5, -0.5))
 world.add(Cube(red_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(0, 0, 4))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(2, 2, 2).translate(4, 0, 4))
 world.add(Cube(purple_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3, 3, 3).translate(7.5, 0.5, 4))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3, 3, 3).translate(-0.25, 0.25, 8))
 world.add(Cube(blue_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(4, 1, 7.5))
 world.add(Cube(red_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3, 3, 3).translate(10, 2, 7.5))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(2, 2, 2).translate(8, 2, 12))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(2, 2, 2).translate(20, 1, 9))
 world.add(Cube(blue_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(-0.5, -5, 0.25))
 world.add(Cube(red_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(4, -4, 0))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(8.5, -4, 0))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(0, -4, 4))
 world.add(Cube(purple_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(-0.5, -4.5, 8))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(0, -8, 4))
 world.add(Cube(white_material).translate(1, -1, 1).scale(0.5, 0.5, 0.5).scale(3.5, 3.5, 3.5).translate(-0.5, -8.5, 8))

 # Render
 measurement = Measurement()
 canvas = camera.render(world)
 println('Rendered in {}ms', measurement.elapsed_time())
 canvas.write('cover_image')

)");

    auto item = lexer.lex();

    while (item.t != type::unknown) {
      std::cout << item.c << " at " << item.loc.start << ":" << item.loc.end << std::endl;
      item = lexer.lex();
    }
  } catch(const std::exception& ex) {
    std::cerr << "lexer error: " << ex.what() << std::endl;
  }

  return 0;
}
