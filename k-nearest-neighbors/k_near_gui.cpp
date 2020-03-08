#include "k_near_gui.h"

template<typename T>
bool within_range(const sf::Vector2i& position, std::vector<std::vector<bool>>& active_positions) {
  if(std::is_same<T, int>()) {
    if(position.x < active_positions[0].size() && position.x > 0 &&
       position.y < active_positions.size() && position.y > 0) {
      return true;
    } else { return false; }
  } else if(std::is_same<T, float>()) {
    if(position.x > 0 && position.x < 1000 && // TO DO : Think about hardcoded size here
       position.y > 0 && position.y < 1000) {
      return true;
    } else { return false; }
  } else { return false; }
}

// Add point in state.selected and in active_positions ( if mode is int ) at position
template<typename T>
void add_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    if constexpr(std::is_same<T, int>()) {
      if(active_positions[position.y][position.x]) return;
      else active_positions[position.y][position.x] = true;
    }
    state->points.emplace_back(position.x, position.y);
    state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
    state->undo_vec.emplace_back(added, *(state->points.end() - 1));
    ++(state->undo_pos);
  }
}

// Delete point in state.selected and in active_positions (i if mode is int ) at position
template<typename T>
void del_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    if constexpr(std::is_same<T, int>()) {
      if(!active_positions[position.y][position.x]) return;
      else active_positions[position.y][position.x] = false;
    }
    state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
    if constexpr(std::is_same<T, int>()) {
      state->undo_vec.emplace_back(deleted, Point<int>(position.x, position.y));
      ++(state->undo_pos);
      state->points.erase(std::remove(state->points.begin(), state->points.end(),
                                      Point<int>(position.x, position.y)), state->points.end());
    } else {
      Point<float> del_pos = Point<float>(position.x, position.y);
      auto idx_nearest = k_nearest<float>(state->points, del_pos, 1);
      if(!state->points.empty() && metric(del_pos, state->points[idx_nearest[0]]) < 8) {
        state->undo_vec.emplace_back(deleted, *(state->points.begin()+idx_nearest[0]));
        ++(state->undo_pos);
        state->points.erase(state->points.begin()+idx_nearest[0],
                            state->points.begin()+idx_nearest[0] + 1);
      }
    }
  }
}

template<typename T>
void sel_op(std::shared_ptr<Mode_State<T>> state, const sf::Vector2i& position,
            std::vector<std::vector<bool>>& active_positions) {
  if(within_range<T>(position, active_positions)) {
    state->selected = Point<T>(position.x, position.y);
  }
}

template<typename T>
void clr_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  state->undo_vec.erase(state->undo_vec.begin() + state->undo_pos + 1, state->undo_vec.end());
  state->undo_vec.emplace_back(cleared, state->points);
  ++(state->undo_pos);
  state->points.clear();
  if constexpr(std::is_same<T, int>())
    for(auto& rows : active_positions) std::fill(rows.begin(), rows.end(), false);
}
template<typename T>
void undo_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  if(state->undo_pos > -1) {
    switch(state->undo_vec[state->undo_pos].what) {
      case added:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos].points[0].y()]
                          [state->undo_vec[state->undo_pos].points[0].x()] = false;
        state->points.erase(state->points.end() - 1, state->points.end());
        break;
      case deleted:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos].points[0].y()]
                          [state->undo_vec[state->undo_pos].points[0].x()] = true;
        state->points.push_back(state->undo_vec[state->undo_pos].points[0]);
        break;
      case cleared:
        state->points = state->undo_vec[state->undo_pos].points;
        if constexpr(std::is_same<T, int>())
          for(const auto& point : state->points) active_positions[point.y()][point.x()] = true;
        break;
    }
    --(state->undo_pos);
  }
}
template<typename T>
void redo_op(std::shared_ptr<Mode_State<T>> state, std::vector<std::vector<bool>>& active_positions) {
  if(state->undo_pos + 1 < state->undo_vec.size()) {
    switch(state->undo_vec[state->undo_pos + 1].what) {
      case added:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos + 1].points[0].y()]
                          [state->undo_vec[state->undo_pos + 1].points[0].x()] = true;
        state->points.push_back(state->undo_vec[state->undo_pos + 1].points[0]);
        break;
      case deleted:
        if constexpr(std::is_same<T, int>())
          active_positions[state->undo_vec[state->undo_pos + 1].points[0].y()]
                          [state->undo_vec[state->undo_pos + 1].points[0].x()] = false;
        state->points.erase(state->points.end() - 1, state->points.end());
        break;
      case cleared:
        state->points.clear();
        if constexpr(std::is_same<T, int>())
          for(auto& rows : active_positions) std::fill(rows.begin(), rows.end(), false);
        break;
    }
    ++(state->undo_pos);
  }
}
template<typename T>
void K_Near_Gui::draw_mode(sf::RenderWindow& window, std::shared_ptr<Mode_State<T>> state,
                           size_t neighbors) const {
  if constexpr(std::is_same<T, int>()) {
    const size_t box_size = 50, border_size = 1; // TO DO
    sf::RectangleShape int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size));
    int_box.setOutlineColor(sf::Color::Yellow);
    size_t border_count = 0, inside_count = 0;
    // Draw Grid // TO DO : Yellow outine instead of yellow box?
    for(int i = 1; i < active_positions.size(); ++i) {
      for(int j = 1; j < active_positions[i].size(); ++j) {
        int_box.setPosition(j * box_size + border_size, i * box_size + border_size);
        active_positions[i][j] ? int_box.setFillColor(sf::Color::Green) :
                                 int_box.setFillColor(sf::Color::Red);
        int dist = metric(Point(j, i), state->selected);
        if(dist == update_.radius) {// TO DO : Fill in option?
          int_box.setFillColor(active_positions[i][j] ? sf::Color::Blue : sf::Color::Yellow);
          if(active_positions[i][j]) ++border_count;
        } else if(dist < update_.radius) {
          if(active_positions[i][j]) ++inside_count;
        }
        window.draw(int_box);
      }
    }
    // Draw k nearest points
    for(auto val : update_.k_near) {
      int_box.setPosition(state->points[val].x() * box_size + border_size,
                          state->points[val].y() * box_size + border_size);
      int_box.setFillColor(sf::Color::Blue);
      window.draw(int_box);
    }
    if(inside_count + border_count > neighbors) {
      gui.set_text(popup(), std::to_string(inside_count + border_count) +
                            " points highlighted inside radius because " +
                            std::to_string(border_count) + " points share the same max distance");
    } else {
      gui.set_text(popup(), "");
    }
    // Draw Selected Element ( fixes border issues ) // TO DO : Stop circle before moved
    if(state->selected.x() > 0 && state->selected.x() < active_positions[0].size() &&
       state->selected.y() > 0 && state->selected.y() < active_positions.size()) {
      int_box.setPosition(state->selected.x() * box_size + border_size,
                          state->selected.y() * box_size + border_size);
      active_positions[state->selected.y()][state->selected.x()] ?
          int_box.setFillColor(neighbors > 0 ? sf::Color::Blue : sf::Color::Green) :
          int_box.setFillColor(sf::Color::Red);
      int_box.setOutlineThickness(2 * border_size);
      window.draw(int_box);
      int_box.setOutlineThickness(0);
    }
  }
  if constexpr(std::is_same<T, float>()) {
    gui.set_text(popup(), "");
    sf::CircleShape nearest_circle(update_.radius);
    nearest_circle.setOrigin(update_.radius, update_.radius);
    nearest_circle.setFillColor(sf::Color::Blue);
    nearest_circle.setOutlineThickness(2);
    nearest_circle.setOutlineColor(sf::Color::Red);
    nearest_circle.setPosition(state->selected.x(), state->selected.y());
    window.draw(nearest_circle);

    sf::CircleShape point_shape(4);
    point_shape.setOrigin(4, 4);
    point_shape.setFillColor(sf::Color::Green);
    for(const auto& point : state->points) {
      point_shape.setPosition(point.x(), point.y());
      window.draw(point_shape);
    }
    point_shape.setPosition(state->selected.x(), state->selected.y());
    point_shape.setFillColor(sf::Color::Yellow);
    window.draw(point_shape);

    // TO DO : Draw nearest
  }
}
K_Near_Gui::K_Near_Gui(sf::RenderWindow& window, const sf::Font& font, size_t neighbors):
  gui(window, font),
  int_button(120, 100, 1030, 50, "INT", true),
  float_button(120, 100, 1150, 50, "FLOAT"),
  k_value(140, 100, 1080, 250, std::to_string(neighbors),
          Base_Scheme.background_ * sf::Color(200, 200, 200)),
  popup(1300, 24, 0, 1000, ""),
  k_decrease(50, 100, 1030, 250, "<"),
  k_increase(50, 100, 1220, 250, ">"),
  add_mode(70, 70, 1030, 450, "ADD"),
  del_mode(70, 70, 1115, 450, "DEL"),
  sel_mode(70, 70, 1030, 570, "SEL"),
  undo(70, 70, 1200, 450, "UNDO"),
  redo(70, 70, 1200, 570, "REDO"),
  clear(70, 70, 1114, 570, "CLR"),
  undo_view({ Text_Display(240, 50, 1030, 680, "", undo_view_color * sf::Color(0, 0, 0, 150)),
              Text_Display(240, 50, 1030, 760, "", undo_view_color),
              Text_Display(240, 50, 1030, 820, "", undo_view_color),
              Text_Display(240, 50, 1030, 880, "", undo_view_color),
              Text_Display(240, 50, 1030, 940, "", undo_view_color) }),
  int_lhs(window, font),
  active_positions(1000 / box_size, std::vector<bool>(1000 / box_size, false)),
  int_box(sf::Vector2f(box_size - 2 * border_size, box_size - 2 * border_size)),
  float_lhs(window, font)
  {
  //gui.add_background(Background(300, 1000, 1000, 0)); TO DO
  Background back(300, 1000, 1000, 0);
  gui.add_background(back);
  gui.add_linked_toggle(int_button, float_button);
  Text_Display neighbor_label(240, 50, 1030, 200, "Neighbors");
  gui.add_text(neighbor_label, k_value, popup);
  gui.add_push_button(k_decrease, k_increase);
  gui.add_linked_toggle(add_mode, del_mode);
  gui.add_toggle_button(sel_mode);
  gui.add_push_button(undo, redo, clear);
  sf::Color undo_view_color(133, 133, 133);
  Text_Display redo_label(240, 20, 1030, 660, "Redo");
  Text_Display undo_label(240, 20, 1030, 740, "Undo");
  gui.add_text(redo_label, undo_label);
  for(auto& elem : undo_view) { gui.add_text(elem); }
  gui.set_text_color(sf::Color::Black);
  Background int_back(1000, 1000, 0, 0, sf::Color::Black);
  int_lhs.add_background(int_back);
  for(int i = 1; i < 1000 / box_size; ++i) {
    Text_Display horz_label(box_size - 2 * border_size, box_size - 2 * border_size,
                            i * box_size + border_size, border_size, std::to_string(i));
    Text_Display vert_label(box_size - 2 * border_size, box_size - 2 * border_size,
                            border_size, i * box_size + border_size, Alphas.substr(i - 1, 1));
    int_lhs.add_text(horz_label, vert_label);
  }
  Text_Display tl_block(box_size - 2 * border_size, box_size - 2 * border_size,
                        border_size, border_size, "");
  int_lhs.add_text(tl_block);
  /* TO DO : Only after implimenting GUI lock features
  std::vector<std::vector<Toggle_Button>> int_positions(1000 / box_size);
  for(size_t i = 1; i < 1000 / box_size; ++i) {
    for(size_t j = 1; j < 1000 / box_size; ++j) {
      int_positions[i].emplace_back(box_size - 2 * border_size, box_size - 2 * border_size,
                            j * box_size + border_size, i * box_size + border_size, "");
      int_lhs.add_toggle_button(*(int_positions[i].end()-1));
    }
  }*/
  for(int i = 0; i < 1000; i+= 100) {
    Background tick_v(1, 10, i, 0, background);
    Background tick_h(10, 1, 0, i, background);
    float_lhs.add_background(tick_v, tick_h);
    Text_Display text_v(20, 16, i, 0, std::to_string(i / 100), sf::Color(0,0,0,0));
    Text_Display text_h(20, 16, 0, i, std::to_string(i / 100), sf::Color(0,0,0,0));
    float_lhs.add_text(text_v, text_h);
  }
  float_lhs.set_text_color(background);
}

Mode K_Near_Gui::mode() const {
  if(gui_state.at(int_button())) return int_;
  else if(gui_state.at(float_button())) return float_;
  else return none_;
}

template<typename T>
void K_Near_Gui::update_mode(sf::RenderWindow& window, std::shared_ptr<Mode_State<T>> state,
                             size_t& neighbors) {
  if(gui_state[k_increase()]) gui.set_text(k_value(), std::to_string(++neighbors));
  if(gui_state[k_decrease()] && neighbors > 0) gui.set_text(k_value(), std::to_string(--neighbors));

  int position_scale = std::is_same<T, int>() ? 50 : 1;
  if(gui_state[add_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if(std::is_same<T, float>()) while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    add_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[del_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    if(std::is_same<T, float>()) while(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {}
    del_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[sel_mode()] && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    sel_op<T>(state, sf::Mouse::getPosition(window) / position_scale, active_positions);
  }
  if(gui_state[clear()]) clr_op<T>(state, active_positions);
  if(gui_state[undo()]) undo_op<T>(state, active_positions);
  if(gui_state[redo()]) redo_op<T>(state, active_positions);
//  update_ = update_state<T>(state, active_positions, neighbors, gui, undo_view);
  update_.k_near = k_nearest(state->points, state->selected, neighbors);
  update_.radius = 0;
  for(auto val : update_.k_near) {
    auto dist = metric(state->points[val], state->selected);
    if(dist > update_.radius) update_.radius = dist;
  }
  if(state->undo_pos + 1 < state->undo_vec.size())
    gui.set_text(undo_view[0](), state->undo_vec[state->undo_pos + 1].to_string());
  else  gui.set_text(undo_view[0](), "");
  for(int i = 0; i < 4; ++i) {
    gui.set_text(undo_view[i + 1](),
                 i <= state->undo_pos ? state->undo_vec[state->undo_pos - i].to_string() : "");
  }
}

void K_Near_Gui::update(sf::RenderWindow& w,
                        std::map<Mode, std::shared_ptr<Mode_Base>>& state_map,
                        size_t& neighbors) {
  gui_state = gui.get_state();
  Mode curr_mode = mode();
  if(curr_mode == int_)
    update_mode<int>(w, state_cast<int>(state_map[int_]), neighbors);
  else if(curr_mode == float_)
    update_mode<float>(w, state_cast<float>(state_map[float_]), neighbors);
}

void K_Near_Gui::draw(sf::RenderWindow& window,
                      const std::map<Mode, std::shared_ptr<Mode_Base>>& state_map,
                      size_t neighbors) const {
  Mode curr_mode = mode();
  if(curr_mode == int_) {
    int_lhs.draw(window);
    draw_mode<int>(window, state_cast<int>(state_map.at(int_)), neighbors);
  } else if(curr_mode == float_) {
    draw_mode<float>(window, state_cast<float>(state_map.at(float_)), neighbors);
    float_lhs.draw(window);
  }
  gui.draw(window);
}

