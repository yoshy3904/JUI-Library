#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

namespace jui
{
    class Canvas;

    enum AnchorPoint 
    {   
        TopLeft, Top, TopRight, 
        Left, Middle, Right, 
        BottomLeft, Bottom, BottomRight 
    };

    enum Orientation { Horizontal, Vertical };

    /*
    Widgets define a space in which UI elements can be drawn. Everything that is a UI element derives from this class. Function overrides should call the base class implementation.
    */
    class Widget
    {
    private:
        sf::RectangleShape background;

        sf::Vector2f position;
        sf::Vector2f size;

        bool is_visible = true; 
        bool is_updated = true; 
        bool is_selectable = true;

        int layer = 0;
    protected:
        sf::View view;
        Widget* mask = nullptr;
        
        sf::RenderWindow& window;
        Canvas& canvas;
    public:
        bool is_draggable = false;
        bool is_resizable = false;
    public:
        Widget(Canvas& p_canvas);
        virtual ~Widget();

        virtual void move(const sf::Vector2f& p_offset);
        // Call base class when overriding!
        virtual void setPosition(const sf::Vector2f& p_position);
        // Call base class when overriding!
        virtual void setSize(const sf::Vector2f& p_size);
        virtual sf::Vector2f getPosition() const;
        virtual sf::Vector2f getSize() const;

        // Methode for drawing. You can never be sure which view your drawing on when entering this function (call "window.setView(view);"" for widget's view). Call base class when overriding!
        virtual void draw() const;
        // Methode for updating the logic of the widget. The canvas will call this methode always.
        virtual void updateLogic();
        // Methode for receiving and handling SFML events.
        virtual void updateEvents(sf::Event& p_event);

        // Whether updatedLogic() and updateEvents() and draw() should be called by the canvas.
        virtual void setVisible(bool p_state);
        bool isVisible();
        // Whether updatedLogic() and updateEvents() should be called by the canvas.
        virtual void setUpdated(bool p_state);
        bool isUpdated();
        // Canvas.getSelected() will never return this instance if it is not selectable, nor is the user able to select the widget by clicking left on it.
        virtual void setSelectable(bool p_state);
        bool isSelectable();
        /* 
        Set another widget to the mask of this widget. All children follow following three rules:
        1. A child should be drawn to the view of its mask. Therefore parts of the child outside of the mask will be clipped.
        2. A child should only be selectable if the mouse is over the mask's bounds.
        3. A child that is outside of its mask needs to resize its own view, so that the view is kept inside the mask's bounds. This applies the clipping of outer masks to this widget.
        */
        virtual void setMask(Widget* p_mask);
        // Layer determines the order in which the widgets are drawn.
        virtual void setLayer(int layer_index);
        int getLayer();

        // Check if the mouse is over this widget. The widget's view is used to get the mouse position, though this does not differ from using the canvas view.
        bool mouseOver();

        const sf::View& getView() const;

        // Layouts and design.

        void setOutlineColor(const sf::Color& p_color);
        void setOutlineThickness(float p_thickness);
        void setFillColor(const sf::Color& p_color);
        
        // @param p_texture The texture needs to stay alive as long as the widget is using it.
        // @param p_texture_rect The part of the texture to be displayed on the widget.
        void setTexture(const sf::Texture* p_texture, const sf::IntRect& p_texture_rect);     
        void setTexture(const sf::Texture* p_texture);
 
        // Orient the position of this widget to another one.
        void orient(AnchorPoint p_point, const Widget& p_widget, const sf::Vector2f& p_offset = sf::Vector2f());

        // Stretch the left, right, top or bottom edge of a widget to another widget with a offset.
        void stretch(AnchorPoint p_point, const Widget& p_widget, float p_offset = 0.f);
    private:
        // Changing the widget's view depending on the 'position' and 'size' class members, while making sure the view will not exceed the parent's bounds.
        void refreshView();
    };

    /* A parent widget allows to group widgets that all get updated the same way, meaning visible, update, selectable states; translation; and events are the same.*/
    // class ParentWidget : public Widget
    // {
    // private:
    //     std::vector<Widget*> children;
    // public:
    //     ParentWidget(Canvas& p_canvas);
    //
    //     void setPosition(const sf::Vector2f& p_position) override;
    //     void setVisible(bool p_state) override;
    //     void setUpdated(bool p_state) override;
    //     void setSelectable(bool p_state) override;
    //     void setOnSelectEvent(const std::function<void()>& p_event) override;
    //     void setOnDeselectEvent(const std::function<void()>& p_event) override;
    // protected:
    //     void addChild(Widget* p_widget);
    //     void removeChild(Widget* p_widget);
    //     Widget* get(int p_index);
    //     int getCount();
    // };

    class Text : public sf::Drawable, public sf::Transformable
    {
    public:
        enum Style
        {
            Regular       = 0,     
            Bold          = 1 << 0,
            Italic        = 1 << 1,
            Underlined    = 1 << 2,
            StrikeThrough = 1 << 3 
        };
    private:
        sf::String string;              
        const sf::Font* font;                
        unsigned int character_size;       
        float letter_spacing_factor; 
        float line_spacing_factor;   
        sf::Uint32 style;               
        sf::Color fill_color;           
        sf::Color selection_color;
        sf::Color outline_color;        
        float outline_thickness;    
        mutable sf::VertexArray vertices;            
        mutable sf::VertexArray outline_vertices;     
        mutable sf::VertexArray selection_vertices;
        mutable sf::FloatRect bounds;              
        mutable bool geometry_need_update;  
    public:
        Text();
        Text(const sf::String& p_string, const sf::Font& p_font, unsigned int p_character_size = 30);

        void setString(const sf::String& p_string);
        void setFont(const sf::Font& p_font);

        void setCharacterSize(unsigned int p_size);
        void setLineSpacing(float p_spacing_factor);
        void setLetterSpacing(float p_spacing_factor);
        void setStyle(sf::Uint32 p_style);

        void setFillColor(const sf::Color& p_color);
        void setOutlineColor(const sf::Color& p_color);
        void setOutlineThickness(float p_thickness);

        void setSelection(int p_start_char, int p_end_char);
        void setSelectionColor(const sf::Color& p_color);
        const sf::Color& getSelectionColor();

        const sf::String& getString() const;
        const sf::Font* getFont() const;

        unsigned int getCharacterSize() const;
        float getLetterSpacing() const;
        float getLineSpacing() const;
        sf::Uint32 getStyle() const;

        const sf::Color& getFillColor() const;
        const sf::Color& getOutlineColor() const;
        float getOutlineThickness() const;

        sf::Vector2f findCharacterPos(std::size_t p_index) const;

        sf::FloatRect getLocalBounds() const;
        sf::FloatRect getGlobalBounds() const;

    private:
        virtual void draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const;
        void ensureGeometryUpdate() const;
        void addLine(sf::VertexArray& p_vertices, float p_line_length, float p_line_top, const sf::Color& p_color, float p_offset, float p_thickness, float p_outline_thickness = 0) const;
        void addGlyphQuad(sf::VertexArray& p_vertices, sf::Vector2f p_position, const sf::Color& p_color, const sf::Glyph& p_glyph, float p_italicShear, float p_outline_thickness = 0) const;
    };

    class TextField : public Widget
    {
    public:
        sf::Text text;
    private:
        sf::Font& font;
        //Icon icon;
        AnchorPoint icon_position = Left;
    public:
        TextField(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void orientText(AnchorPoint p_anchor, const sf::Vector2f& p_offset = sf::Vector2f()/*, bool p_allow_overflow*/);

        //void setIcon(const sf::Texture* p_texture, AnchorPoint p_position = Left, const sf::Vector2f& p_size = sf::Vector2f(32.f, 32.f));

        void draw() const override;
    };

    class Button : public Widget
    {
    private:
        std::function<void()> f_event;
        TextField text_field;
    public:
        sf::Color highlight_color = sf::Color(100, 100, 100);
        sf::Color standard_color = sf::Color::White;
        sf::Color pressed_color = sf::Color(50, 50, 50);
    public:
        Button(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void setOnClickEvent(const std::function<void()>& p_event);

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;

        void setString(const sf::String& p_string);
        const sf::String& getString();
        void orientText(AnchorPoint p_orientation, const sf::Vector2f& p_offset = sf::Vector2f());
        void setTextColor(const sf::Color& p_color);

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;
    };

    class ScrollBar : public Widget
    {
    public:
        struct ScrollState 
        {
            // Defines the visible part of the list in pixels.
            float scroll_handle_position;
            float scroll_handle_size;
            // Defines the lists whole length in pixels.
            float scroll_list_length;
        };
    private:
        Button move_up_button;
        Button move_down_button;
        sf::RectangleShape scroll_handle;
        Orientation orientation;
        ScrollState state;
        bool handle_selected = false;
        sf::Vector2f last_mouse_position;
        std::function<void()> f_onScroll = [](){};
    public:
        ScrollBar(Canvas& p_canvas, sf::Font& p_font, Orientation p_orientation); 

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void setScrollState(float p_position, float p_size, float p_list_length);
        const ScrollState& getScrollState();

        void setOnScrollListener(const std::function<void()>& p_event);

        void draw() const override;
        void updateEvents(sf::Event& p_event) override;
        void updateLogic() override;

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;
    private:
        void refreshIcons(); 
        void refreshPosition();
        void refreshSize();
    };

    /* A standard layout group that holds a list of pointers to widgets. It allows grouped movement, and layouting of the added widgets.*/
    class LayoutGroup : public Widget
    {
    private:
        std::vector<Widget*> widget_list;
    public:
        LayoutGroup(Canvas& p_canvas);

        void setPosition(const sf::Vector2f& p_position) override;
        sf::Vector2f getGroupSize() const;

        void listWidgets(Orientation p_orientation, float p_spacing = 10.f);

        void setMask(Widget* p_widget) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;

        void add(Widget* p_widget);
        void remove(Widget* p_widget);
        Widget* get(int p_index);
        int getCount();
    };

    class ScrollList : public Widget
    {
    private:
        ScrollBar scroll_bar;
        LayoutGroup content;
    public:
        ScrollList(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;

        void add(Widget* p_widget);
        void remove(Widget* p_widget);
        Widget* get(int p_index);
        int getCount();

        void listWidgets(Orientation orientation);

        void updateEvents(sf::Event& p_event) override;
    };

    class DropDownList : public Widget
    {
    private:
        Button dropdown_button;
        ScrollList scrolllist;
        sf::Font& font;
    public:
        DropDownList(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void updateEvents(sf::Event& p_event) override;

        void setMask(Widget* p_widget) override;
        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;

        void setString(const sf::String& p_string);
        sf::String getString();

        void setButtonColor(const sf::Color& p_color);
        void setButtonTextColor(const sf::Color& p_color);

        void setList(const std::vector<sf::String>& p_elements);
        void setListLength(float p_length);
    };

    class InputField : public Widget
    {
    public:
        enum Type 
        { 
            Standard                = 1 << 0, 
            IntegerNumber           = 1 << 1, 
            DecimalNumber           = 1 << 2, 
            HexadecimalNumber       = 1 << 3, 
            BinaryNumber            = 1 << 4, 
            Alphanumeric            = 1 << 5,
            Alpha                   = 1 << 6,
            
            Date                    = 1 << 7, 
            LimitedValue            = 1 << 8, 
            Capitalized             = 1 << 9,
            Custom                  = 1 << 10
        };
        sf::Color highlight_color = sf::Color(100, 100, 100);
        sf::Color standard_color = sf::Color::White;
        sf::Color error_color = sf::Color(255, 100, 100);
        sf::Color placeholder_text_color = sf::Color(100, 100, 100);
        sf::Color text_selection_color = sf::Color(0, 0, 255, 100);
    private:
        sf::Text text;
        sf::Text placeholder_text;
        sf::RectangleShape cursor;
        sf::RectangleShape selection;
        int cursor_index = 0;
        int drag_cursor_index = 0;
        bool is_making_mouse_selection = false;
        bool is_making_shift_selection = false;
        int max_character_limit = 1000;
        int min_character_limit = 0;
        float max_value_limit = 0.f;
        float min_value_limit = 1000.f; 

        bool limit_error = false;
        bool input_error = false;
        bool date_error = false;
        sf::Uint32 type = Type::Standard;
        std::function<bool(sf::Uint32 p_char)> f_customIsCharValid;

        std::function<void()> f_onSelect;
        std::function<void()> f_onDeselect;
        bool was_selected = false;
        bool was_hovered = false;
    public:
        InputField(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;
        void setCharacterSize(unsigned int p_size);
        void setInputType(sf::Uint32 p_type);
        /*How many characters there can be in the inputfield at minimum and maximum.*/
        void setCharacterLimit(int p_min, int p_max);
        /*If the inputfield expects number values, these are the smallest and biggest allowed numbers.*/
        void setValueLimit(float p_min, float p_max);
        void setString(const sf::String& p_string);
        const sf::String& getString();
        bool isInputValid();
        /*Lets the user write a function that takes the inputted char in and outputs whether that char is valid. Set the type to Custom to use this.*/
        void setValidChars(const std::function<bool(sf::Uint32 p_char)>& p_function);
        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);
        void setPlaceholderText(const sf::String& p_string);

        void draw() const override;
        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;
    private:
        void refreshCursor();
        void refreshSelection();
        bool isCharValid(sf::Uint32 p_char);
        void refreshDateFormat();
        void refreshLimitedFormat();
        #ifdef WINDOWS
        sf::String getClipboardText();
        void copyToClipboard(const sf::String& p_string);
        #endif
    };

    class Tabs : public Widget
    {
    private:
        struct Tab
        {
            Widget* widget;
            Button* button;
            sf::String name;
        };
        std::vector<Tab> tab_list;

        sf::RectangleShape button_background;
        float tabbar_height;
        float horizontal_spacing;
        float left_bar_margin, right_bar_margin, top_bar_margin, bottom_bar_margin; 
        sf::Font& font;
    public:
        sf::Color selected_color = sf::Color::White;
        sf::Color standard_color = sf::Color(150, 150, 150);
    public:
        Tabs(Canvas& p_canvas, sf::Font& p_font);
        ~Tabs();

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void setBarColor(const sf::Color& p_color);
        void setBarHeight(float p_height);
        void setBarMargin(float p_left, float p_right, float p_top, float p_bottom);

        void showTab(int p_index);

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;

        void draw() const override;

        void add(Widget* p_widget, const sf::String& p_name);
        void remove(int p_index);
    private:
        void refreshUI();
    };

    /*
    A canvas draws, updates the logic, the events, and handles the layout of widgets. A canvas needs a reference to a window in order to know where it has to draw its widgets.
    */
    class Canvas
    {
    public:
        sf::RenderWindow& window;

        std::vector<Widget*> widget_list;
    private:
        sf::Color selected_color = sf::Color(100, 100, 255);
        Widget* selected_widget = nullptr;
        Widget* hovered_widget = nullptr;
        
        // The view of the canvas always shows the 2d world that fits into the window with origin in the top left corner. It should be used for getting the mouse position.
        // The views of all widgets act the same, only that they are showing a small part of the window. Viewport and depicted part of the 2d world always overlay.
        sf::View view;
    
        // Resizing and dragging. // TODO: Move this into seperate class. Most widgets don't need this.
        bool is_dragging = false;
        sf::Vector2f last_mouse_position;
        bool is_resizing = false;
        int resizing_edge;
        sf::Vector2f resize_origin;

        float* delta_time = nullptr;
    public:
        Canvas(sf::RenderWindow& p_window);

        void drawAll();
        void updateAllLogic();
        void updateAllEvents(sf::Event& p_event);

        void add(Widget* p_widget);
        void insert(Widget* p_widget, int p_index);
        int find(Widget* p_widget);
        Widget* get(int p_index);
        int getCount();
        // Find index of widget in list. If the widget is not stored, -1 is returned.
        void remove(Widget* p_widget);
    
        // Get a pointer to the selected widget. If nothing is selected, nullptr is returned.
        Widget* getSelected();
        // Get a pointer to the top most widget the mouse is currently hovering over.
        Widget* getHovered();
        // Get the view of this canvas.
        const sf::View& getView() const;

        void select(Widget* p_widget);

        void print();

        void setDeltaTime(float& p_delta_time);
        float getDeltaTime();

    private:
        void drag();
        void resize();
    };
};
