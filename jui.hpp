#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream> /* debugging */
#include <sstream>

namespace jui
{
    class Canvas;

    enum AnchorPoint { TopLeft, Top, TopRight, Left, Middle, Right, BottomLeft, Bottom, BottomRight };
    enum Orientation { Horizontal, Vertical };

    /*
    Widgets define a space in which UI elements can be drawn. Everything that is a UI element derives from this class. Overridden functions should call the base class implementation.
    */
    class Widget
    {
    private:
        sf::Vector2f position;
        sf::Vector2f size;

        bool is_visible = true; 
        bool is_updated = true; 
        bool is_selectable = true;

        int layer = 0;

        Widget* mask = nullptr;
        std::vector<Widget*> children;
    protected:
        sf::RectangleShape background;

        sf::View view;
        sf::RenderWindow& window;
        Canvas& canvas;
    public:
        Widget(Canvas& p_canvas);
        virtual ~Widget();

        void move(const sf::Vector2f& p_offset);
        virtual void setPosition(const sf::Vector2f& p_position);
        virtual void setSize(const sf::Vector2f& p_size);
        virtual sf::Vector2f getPosition() const;
        virtual sf::Vector2f getSize() const;

        // Methode for drawing. You can never be sure which view your drawing on when entering this function (call "window.setView(view);" for the widget's own view, but in general it is better to only use widgets and then use setMask(), instead of overriding this function).
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
        Set another widget to the mask of this widget. All widgets follow following three rules:
        1. A widget should be drawn to the view of its mask. Therefore parts of the widget outside of the mask will be clipped.
        2. A widget should only be selectable if the mouse is over the mask's bounds.
        3. A child that is outside of its mask needs to resize its own view, so that the view is kept inside the mask's bounds. This applies the clipping of outer masks to this widget.
        */
        virtual void setMask(Widget* p_mask);
        Widget* getMask() const;
        // Layer determines the order in which the widgets are drawn. If widgets have the same layer, they will be drawn in the order the layer was set.
        virtual void setLayer(int layer_index);
        int getLayer();

        void addChild(Widget* p_widget);
        void removeChild(Widget* p_widget);
        Widget* getChild(int p_index);
        int getChildCount();

        // Check if the mouse is over this widget.
        bool mouseOver();

        const sf::View& getView() const;

        // LAYOUTS AND DESIGN.

        void setOutlineColor(const sf::Color& p_color);
        void setOutlineThickness(float p_thickness);
        void setFillColor(const sf::Color& p_color);
        
        // @param p_texture The texture needs to stay alive as long as the widget is using it.
        // @param p_texture_rect The part of the texture to be displayed on the widget.
        void setTexture(const sf::Texture* p_texture, const sf::IntRect& p_texture_rect);     
        void setTexture(const sf::Texture* p_texture);

        void setAnchor(const sf::FloatRect& p_anchor);
        const sf::FloatRect& getAnchor();
    private:
        // Changing the widget's view depending on the 'position' and 'size' class members, while making sure the view will not exceed the parent's bounds.
        void refreshView();
    };

    class Graphic : public Widget
    {
    private:
        bool preserve_aspect = true;
    public:
        Graphic(Canvas& p_canvas);

        void setPreserveAspect(bool p_state);
        bool isPreservingAspect();

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;
    };

    class TextField : public Widget
    {
    public:
        enum Alignment
        {
            Left, Centered, Right, Top, Middle, Bottom
        };
        enum OverflowType
        {
            Overflow, Truncate, Dotted
        };
    private:
        sf::String string;
        sf::Font& font;
        unsigned int character_size;
        float line_spacing_factor;
        float paragraph_spacing_factor;
        sf::Color text_color;
        Alignment horizontal_alignment;
        Alignment vertical_alignment;
        OverflowType overflow_type;
        bool wrapping;

        // Stores each line in a vector that stores the position for each character.
        struct CharacterInfo
        {
            // Position on the baseline of the text.
            sf::Vector2f position; 
            // Index of the first and last vertex of the 6 vertices making up the glyph.
            int start_vertex = 0, end_vertex = 0;
        };
        mutable std::vector<std::vector<CharacterInfo>> text_info;
        mutable sf::FloatRect text_bounds;
        sf::Vector2f character_bounds;
        sf::Transform text_transform;
        mutable sf::VertexArray vertices;
        mutable bool geometry_need_update;

        mutable sf::Vector2f start_of_string;
        mutable sf::Vector2f end_of_string;

        // Selection of a specific part of the text.
        int start_selection = 0, end_selection = 0;
        sf::Color selection_color;
        mutable sf::VertexArray selection_vertices;
    public:
        TextField(Canvas& p_canvas, sf::Font& p_font);

        void setString(const sf::String& p_string);
        void setFont(const sf::Font& p_font);
        void setCharacterSize(unsigned int p_size);
        void setLineSpacing(float p_spacing_factor);
        void setParagraphSpacing(float p_sapcing_factor);
        void setTextColor(const sf::Color& p_color);
        void setOverflowType(OverflowType p_type);
        void enableWrapping(bool p_state);
        void setHorizontalAlignment(Alignment p_type);
        void setVerticalAlignment(Alignment p_type);
        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        const sf::String& getString() const;
        const sf::Font& getFont() const;
        unsigned int getCharacterSize() const;
        float getLineSpacing() const;
        float getParagraphSpacing() const;
        const sf::Color& getTextColor() const;
        OverflowType getOverflowType() const;
        bool isWrapping() const;
        Alignment getHorizontalAlignment() const;
        Alignment getVerticalAlignment() const;

        void setSelection(int p_start, int p_end);
        void setSelectionColor(const sf::Color& p_color);
        const sf::Color& getSelectionColor() const;

        sf::Vector2f findCharacterPos(int p_index) const;
        int findIndex(const sf::Vector2f& p_position) const;
    private:
        void ensureGeometryUpdate() const;
        void draw() const override;
        void addGlyphQuad(sf::VertexArray& p_vertices, const sf::Vector2f& p_position, const sf::Color& p_color, const sf::Glyph& p_glyph) const;

        void applyHorizontalAlignment() const;
        void applyVerticalAlignment() const;
        sf::String applyOverflowType() const;
        void applySelection() const;
        void applyStringGeometry(const sf::String& p_string) const;
    };

    class Button : public Widget
    {
    private:
        std::function<void()> f_event;
    public:
        TextField text;
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
        Widget scroll_handle;
        Orientation orientation;
        ScrollState state;
        bool handle_selected = false;
        sf::Vector2f last_mouse_position;
        std::function<void()> f_onScroll = [](){};

        float handle_margin = 0.f;
        sf::Color handle_color = sf::Color(193, 193, 193);
        sf::Color hovered_handle_color = sf::Color(220, 220, 220);
    public:
        ScrollBar(Canvas& p_canvas, sf::Font& p_font, Orientation p_orientation); 

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;

        void setScrollState(float p_position, float p_size, float p_list_length);
        const ScrollState& getScrollState();

        void setOnScrollListener(const std::function<void()>& p_event);

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
        void setLayer(int p_layer) override;

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

        float scrollbar_width = 20.f;
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

        void setScrollBarWidth(float p_width);
        float getScrollBarWidth();

        void listWidgets(Orientation orientation);

        void updateEvents(sf::Event& p_event) override;
    };

    class DropDownList : public Widget
    {
    private:
        Button dropdown_button;
        ScrollList scrolllist;
        sf::Font& font;

        std::function<void()> f_onSelect;
        std::function<void()> f_onDeselect;
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

        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);

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
    private:
        TextField text;
        TextField placeholder_text;
        Widget cursor;
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

        float blinking_timer = 0.f;
        float blinking_interval = 0.5f;

        float text_padding = 5.f;
    public:
        InputField(Canvas& p_canvas, sf::Font& p_font);

        void setPosition(const sf::Vector2f& p_position) override;
        void setSize(const sf::Vector2f& p_size) override;
        void setString(const sf::String& p_string);
        const sf::String& getString();
        void setPlaceholderText(const sf::String& p_string);
        void setCharacterSize(unsigned int p_size);
        void setInputType(sf::Uint32 p_type);
        // How many characters there can be in the inputfield at minimum and maximum.
        void setCharacterLimit(int p_min, int p_max);
        // If the inputfield expects number values, these are the smallest and biggest allowed numbers.
        void setValueLimit(float p_min, float p_max);
        bool isInputValid();
        // Lets the user write a function that takes the inputted char in and outputs whether that char is valid. Set the type to Custom to use this.
        void setValidChars(const std::function<bool(sf::Uint32 p_char)>& p_function);
        void setOnSelectEvent(const std::function<void()>& p_event);
        void setOnDeselectEvent(const std::function<void()>& p_event);

        bool isSelected() const;
        bool isHovered() const;

        void updateLogic() override;
        void updateEvents(sf::Event& p_event) override;

        void setLayer(int p_layer) override;
        void setVisible(bool p_state) override;
        void setUpdated(bool p_state) override;
        void setSelectable(bool p_state) override;
    private:
        void refreshCursor();
        bool isCharValid(sf::Uint32 p_char);
        void refreshDateFormat();
        void refreshLimitedFormat();
        void moveTextHorizontally(int p_index);
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
        Widget* selected_widget = nullptr;
        Widget* hovered_widget = nullptr;
        
        // The view of the canvas always shows the 2d world that fits into the window with origin in the top left corner. It should be used for getting the mouse position. The views of all widgets act the same, only that they are showing a small part of the window. Viewport and depicted part of the 2d world always overlay.
        sf::View view;

        static float s_delta_time;
        sf::Clock clock;
    public:
        Canvas(sf::RenderWindow& p_window);

        void drawAll();
        void updateAllLogic();
        void updateAllEvents(sf::Event& p_event);

        void add(Widget* p_widget);
        void remove(Widget* p_widget);

        // Changes the position of the widget in the widget_list, therefore changing its drawing order. This won't apply the given layer to the widget. Use Widget::setLayer().
        void moveToLayer(Widget* p_widget, int p_layer);
    
        // Get a pointer to the selected widget. If nothing is selected, nullptr is returned.
        Widget* getSelected();
        // Get a pointer to the top most widget the mouse is currently hovering over.
        Widget* getHovered();
        // Get the view of this canvas.
        const sf::View& getView() const;

        void select(Widget* p_widget);

        void print();

        static float getDeltaTime();
    private:
        // Find index of widget in list. If the widget is not stored, -1 is returned.
        int find(Widget* p_widget) const;
    };

    /*
    The ResourceHandler can hold multiple resources that can be accessed through a key.
    */
    template<typename Identifier, typename Item>
    class ResourceHandler
    {
    private:
        std::map<Identifier, Item*> m_resourceTable;
        Item m_errorItem;
    public:
        ~ResourceHandler()
        {
            for(const auto& element : m_resourceTable)
            {
                delete element.second;
            }
            m_resourceTable.clear();
        }
        bool load(Identifier p_identifier, const std::string& p_itemPath)
        {
            Item* item = new Item();
            if(!item->loadFromFile(p_itemPath))
            {
                //throw std::runtime_error("ResourceHandler: Failed to load " + p_itemPath);
                sf::String temp(p_itemPath);
                std::wcerr << L"ResourceHandler: Failed to load " << temp.toWideString() << std::endl;
                return false;
            }
            auto inserted = m_resourceTable.insert(std::make_pair(p_identifier, item));
            if(inserted.second == false)
            {
                //assert(inserted.second) -> calls abort()
                //throw std::logic_error("ResourceHandler: Loaded resource " + p_itemPath + " twice.");
                sf::String temp(p_itemPath);
                std::wcerr << L"ResourceHandler: Loaded resource " << temp.toWideString() << L" twice." << std::endl;
                return false;
            }
            return true;
        }
        Item& get(Identifier p_identifier)
        {
            auto search = m_resourceTable.find(p_identifier);
            if(search == m_resourceTable.end())
            {
                //assert(search != m_resourceTabel.end()) -> calls abort()
                //throw std::logic_error("ResourceHandler: Tried to get resource without loading it.");
                std::wcerr << L"ResourceHandler: Tried to get resource without loading it." << std::endl;
                return m_errorItem;
            }
            return *search->second;
        }
    };

    /*
    A static class for fast and simple font loading and accessing. Fonts are heavy objects and therefore should only be loaded once in the lifetime of a program.
    */
    class Fonts
    {
    private:
        static ResourceHandler<std::string, sf::Font> s_fontTable;
    public:
        static bool load(const std::string& p_name, const std::string& p_filePath);
        static sf::Font& get(const std::string& p_name);
    };

    class Cursor
    {
    private:
        struct CursorData
        {
        public:
            // All cursor available by the system through SFML.
            sf::Cursor system_cursors[13];
        public:
            CursorData();

            void setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
        };
        static CursorData s_data;
    public:
        static void setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
    };

    /*
    Get the current mouse position relative to the window. If no view is provided the view currently used by the window is used. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view = nullptr);
    /*
    Get the current mouse position relative to a point. If no point is specified the window's upper left corner is used. 
    If no view is provided the view currently used by the window is used. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    @param relative_to The point to which the mouse position should be relative to. 
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view = nullptr);

    /*
    Check whether the mouse is over a circle constructed from a center and radius.
    @return True = Mouse over circle, False = Mouse not over circle
    */
    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    Check whether the mouse is over an axis-aligned rectangle.
    @return True = Mouse over rectangle, False = Mouse not over rectangle
    */
    bool mouseOverRectangle(const sf::FloatRect& rectangle, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    Split a string by some character.
    */
    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c);

    template<typename T>
    T clampValue(const T& p_value, const T& p_min, const T& p_max)
    {
        T modified_value = p_value;
        if(modified_value < p_min) modified_value = p_min;
        else if (modified_value > p_max) modified_value = p_max;
        return modified_value;
    }

    sf::String toUppercase(const sf::String& p_string);
    sf::Uint32 toUppercase(sf::Uint32 p_char);

    /* Transform some numeric value into a string with precision. */
    template <typename T>
    sf::String toString(const T p_value, const int p_digits = 6)
    {
        std::ostringstream out;
        out.precision(p_digits);
        out << std::fixed << p_value;
        sf::String replaced_string(out.str());
        replaced_string.replace(".", ",");
        return replaced_string;
    }

    /* Transform a number into a string representing the given number in binary format. */
    sf::String toBinaryString(unsigned int p_value);
    /* Transform a number into a string representing the given number in hexadecimal format. */
    sf::String toHexadecimalString(unsigned int p_value, bool p_uppercase = true);
    /* Transform a string into a decimal number. Dots as well as commas are valid. Invalid strings (containing letters/symbols) return 0. */
    float stringToFloat(const sf::String& p_string);
    /* Transform a string into a integer. Invalid strings (containing letters/symbols) return 0. */
    int stringToInt(const sf::String& p_string);
    /* Transform a string representing a number in hexadecimal format into a number. */
    unsigned int hexadecimalToInt(sf::String p_string);
    /* Transform a string representing a number in binary format into a number. */
    unsigned int binaryToInt(sf::String p_string);
};
