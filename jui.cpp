#include "jui.hpp"
#include <vector>

namespace jui
{
    Widget::Widget(Canvas& p_canvas)
        : position(sf::Vector2f(100.f, 100.f)), 
        size(sf::Vector2f(100.f, 100.f)),
        window(p_canvas.window), 
        canvas(p_canvas)
    {
        setPosition(sf::Vector2f(0.f, 0.f));
        setSize(sf::Vector2f(100.f, 100.f));
        setOutlineThickness(1.f);
        setOutlineColor(sf::Color(170, 170, 170));

        p_canvas.add(this);
    }
    
    Widget::~Widget() 
    {
        canvas.remove(this);
    }
    
    void Widget::move(const sf::Vector2f& p_offset) 
    {
        setPosition(getPosition() + p_offset);
    }
    
    void Widget::setPosition(const sf::Vector2f& p_position) 
    {
        position = p_position;
        background.setPosition(position.x + background.getOutlineThickness(), position.y + background.getOutlineThickness());
        refreshView();
    }
    
    void Widget::setSize(const sf::Vector2f& p_size) 
    {
        size = p_size;
        background.setSize(sf::Vector2f(size.x - background.getOutlineThickness() * 2.f, size.y - background.getOutlineThickness() * 2.f));
        refreshView();
    }
    
    sf::Vector2f Widget::getPosition() const
    {
        return position;
    }
    
    sf::Vector2f Widget::getSize() const
    {
        return size;
    }
    
    void Widget::draw() const
    {
        if(mask != nullptr)
        {
            window.setView(mask->getView());
        }
        else
        {
            window.setView(view);
        }

        // Draw.
        window.draw(background);
    }
    
    void Widget::updateLogic() 
    {
        // If the mask gets resized, the child wouldn't follow, because it isn't updated/positioned. Calling this always makes sure all widgets are up to date. (Solve differently?)
        refreshView();
    }
    
    void Widget::updateEvents(sf::Event& p_event) 
    {
    }
    
    void Widget::setOutlineColor(const sf::Color& p_color) 
    {
        background.setOutlineColor(p_color);
    }
    
    void Widget::setOutlineThickness(float p_thickness)
    {
        background.setOutlineThickness(p_thickness);
        background.setPosition(position.x + background.getOutlineThickness(), position.y + background.getOutlineThickness());
        background.setSize(sf::Vector2f(size.x - background.getOutlineThickness() * 2.f, size.y - background.getOutlineThickness() * 2.f));
    }

    void Widget::setFillColor(const sf::Color& p_color) 
    {
        background.setFillColor(p_color);
    }

    void Widget::setTexture(const sf::Texture* p_texture, const sf::IntRect& p_texture_rect)
    {
        background.setTextureRect(p_texture_rect);
        background.setTexture(p_texture);
    }
    void Widget::setTexture(const sf::Texture* p_texture)
    {
        background.setTexture(p_texture);
    }

    void Widget::setVisible(bool p_state)
    {
        is_visible = p_state;
    }

    bool Widget::isVisible()
    {
        return is_visible;
    }

    void Widget::setUpdated(bool p_state)
    {
        is_updated = p_state;
    }

    bool Widget::isUpdated()
    {
        return is_updated;
    }

    void Widget::setSelectable(bool p_state)
    {
        is_selectable = p_state;
    }

    bool Widget::isSelectable()
    {
        return is_selectable;
    }

    void Widget::setLayer(int p_layer_index)
    {   
        layer = p_layer_index;
        canvas.moveToLayer(this, layer);
    }

    int Widget::getLayer()
    {
        return layer;
    }

    void Widget::addChild(Widget* p_widget)
    {
        // Check if widget was already added as child.
        for (int i = 0; i < children.size(); i++)
        {
            if(p_widget == children[i])
            {
                return;
            }
        }
        // Add child.
        children.push_back(p_widget);
    }

    void Widget::removeChild(Widget* p_widget)
    {
        for (int i = 0; i < children.size(); i++)
        {
            if(p_widget == children[i])
            {
                children.erase(children.begin() + i);
            }
        }
    }

    Widget* Widget::getChild(int p_index)
    {
        if(children.size() == 0) return nullptr;
        if(p_index < 0) p_index = 0;
        if(p_index >= children.size()) p_index = children.size() - 1;
        return children[p_index];
    }

    int Widget::getChildCount()
    {
        return children.size();
    }

    void Widget::refreshView() 
    {
        if(size.x < 0 || size.y < 0)
        {
            view.reset(sf::FloatRect(position, sf::Vector2f()));
	        view.setViewport(sf::FloatRect(position.x/window.getSize().x, position.y/window.getSize().y, 0.f, 0.f));
            return;
        }

        // With mask. This requires to also apply the clipping done by outer masks, more info above the definition of setMask().
        if(mask != nullptr)
        {
            // Store how the view should normally look.
            sf::Vector2f view_position = position;
            sf::Vector2f view_size = size;

            sf::FloatRect mask_bounds = sf::FloatRect(mask->getView().getViewport().left * window.getSize().x, mask->getView().getViewport().top * window.getSize().y,
                                                        mask->getView().getViewport().width * window.getSize().x, mask->getView().getViewport().height * window.getSize().y);

            // Check if the child is outside of the mask, then resize and reposition it to fit the bounds.
            if(mask_bounds.left > position.x)
            {
                float difference = position.x + size.x - mask_bounds.left;
                view_size.x = difference >= 0.f ? difference : 0.f;
                view_position.x = mask_bounds.left;
            }
            if(mask_bounds.left + mask_bounds.width < position.x + size.x)
            {
                float difference = mask_bounds.left + mask_bounds.width - position.x;
                view_size.x = difference >= 0.f ? difference : 0.f;
            }
            if(mask_bounds.top > position.y)
            {
                float difference = position.y + size.y - mask_bounds.top;
                view_size.y = difference >= 0.f ? difference : 0.f;
                view_position.y = mask_bounds.top;
            }
            if(mask_bounds.top + mask_bounds.height < position.y + size.y)
            {
                float difference = mask_bounds.top + mask_bounds.height - position.y;
                view_size.y = difference >= 0.f ? difference : 0.f;
            }
            // In the case that left and right or up and down edges overlap the mask at the same time.
            if((mask_bounds.left > position.x) && (mask_bounds.left + mask_bounds.width < position.x + size.x))
            {
                view_size.x = mask_bounds.width;
                view_position.x = mask_bounds.left;
            }
            if((mask_bounds.top > position.y) && (mask_bounds.top + mask_bounds.height < position.y + size.y))
            {
                view_size.y = mask_bounds.height;
                view_position.y = mask_bounds.top;
            }

            // Apply the new position and size to the view.
            view.reset(sf::FloatRect(view_position, view_size));
	        view.setViewport(sf::FloatRect(view_position.x/window.getSize().x, view_position.y/window.getSize().y, view_size.x/window.getSize().x, view_size.y/window.getSize().y));
            return;
        }

        // Without mask.
        view.reset(sf::FloatRect(position, size));
	    view.setViewport(sf::FloatRect(position.x/window.getSize().x, position.y/window.getSize().y, size.x/window.getSize().x, size.y/window.getSize().y));
    }
    
    bool Widget::mouseOver() 
    {
        bool mouse_over = sf::FloatRect(position, size).contains(getMousePosition(window, &view)); 
        if(mask != nullptr)
        {
            return mouse_over && mask->mouseOver();
        }
        return mouse_over;
    }
    
    void Widget::setMask(Widget* p_mask) 
    {
        mask = p_mask;
    }

    Widget* Widget::getMask() const
    {
        return mask;
    }
    
    const sf::View& Widget::getView() const
    {
        return view;
    }

    Graphic::Graphic(Canvas& p_canvas) : Widget(p_canvas)
    {
        setSize(sf::Vector2f(100.f, 100.f));
    }

    void Graphic::setPreserveAspect(bool p_state)
    {
        preserve_aspect = p_state;
        setSize(getSize());
    }
    bool Graphic::isPreservingAspect()
    {
        return preserve_aspect;
    }

    void Graphic::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        if(preserve_aspect && background.getTexture() != nullptr)
        {
            background.setPosition(getPosition() + sf::Vector2f(getSize().x/2.f - background.getSize().x/2.f, getSize().y/2.f - background.getSize().y/2.f));
        }
    }

    void Graphic::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        if(preserve_aspect && background.getTexture() != nullptr)
        {
            sf::Vector2f base_size = (sf::Vector2f)background.getTexture()->getSize();
            float aspect_ratio = base_size.x / base_size.y;
            if(p_size.y * aspect_ratio < p_size.x)
            {
                background.setSize(sf::Vector2f(p_size.y * aspect_ratio, p_size.y));
            }
            else
            {
                aspect_ratio = base_size.y / base_size.x;
                background.setSize(sf::Vector2f(p_size.x, p_size.x * aspect_ratio));
            }
            background.setPosition(getPosition() + sf::Vector2f(getSize().x/2.f - background.getSize().x/2.f, getSize().y/2.f - background.getSize().y/2.f));
        }
    }

    TextField::TextField(Canvas& p_canvas, sf::Font& p_font) 
        : Widget(p_canvas), 
        string(L"New Text"), 
        font(p_font),
        character_size(20),
        line_spacing_factor(1.f),
        paragraph_spacing_factor(1.5f),
        text_color(sf::Color::Black),
        horizontal_alignment(Left), 
        vertical_alignment(Left), 
        overflow_type(Overflow),
        wrapping(true),
        vertices(sf::Triangles),
        geometry_need_update(true),
        selection_color(sf::Color(0, 0, 255, 100)),
        selection_vertices(sf::Triangles)
    {
        setOutlineColor(sf::Color(170, 170, 170));
        setOutlineThickness(1.f);
    }

    void TextField::setString(const sf::String& p_string)
    {
        if(string != p_string)
        {
            string = p_string;
            geometry_need_update = true;
        }
    }

    void TextField::setFont(const sf::Font& p_font)
    {
        if(&font != &p_font)
        {
            font = p_font;
            geometry_need_update = true;
        }
    }

    void TextField::setCharacterSize(unsigned int p_size)
    {
        if(character_size != p_size)
        {
            character_size = p_size;
            geometry_need_update = true;
        }
    }

    void TextField::setLineSpacing(float p_spacing_factor)
    {
        if(line_spacing_factor != p_spacing_factor)
        {
            line_spacing_factor = p_spacing_factor;
            geometry_need_update = true;
        }   
    }

    void TextField::setParagraphSpacing(float p_spacing_factor)
    {
        if(paragraph_spacing_factor != p_spacing_factor)
        {
            paragraph_spacing_factor = p_spacing_factor;
            geometry_need_update = true;
        }   
    }

    void TextField::setTextColor(const sf::Color& p_color)
    {
        if(text_color != p_color)
        {
            text_color = p_color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!geometry_need_update)
            {
                for (std::size_t i = 0; i < vertices.getVertexCount(); ++i)
                    vertices[i].color = text_color;
            }
        }
    }

    void TextField::setOverflowType(OverflowType p_type)
    {
        if(overflow_type != p_type)
        {
            overflow_type = p_type;
            geometry_need_update = true;
        }
    }

    void TextField::enableWrapping(bool p_state)
    {
        if(wrapping != p_state)
        {
            wrapping = p_state;
            geometry_need_update = true;
        }
    }

    void TextField::setHorizontalAlignment(Alignment p_type)
    {
        if(horizontal_alignment != p_type)
        {
            horizontal_alignment = p_type;
            geometry_need_update = true;
        }
    }

    void TextField::setVerticalAlignment(Alignment p_type)
    {
        if(vertical_alignment != p_type)
        {
            vertical_alignment = p_type;
            geometry_need_update = true;
        }
    }


    void TextField::setPosition(const sf::Vector2f& p_position)
    {
        if(getPosition() != p_position)
        {
            Widget::setPosition(p_position);

            text_transform = sf::Transform::Identity;
            text_transform.translate(p_position);
            geometry_need_update = true;
        }
    }

    void TextField::setSize(const sf::Vector2f& p_size)
    {
        if(getSize() != p_size)
        {
            Widget::setSize(p_size);
            geometry_need_update = true;
        }
    }

    const sf::String& TextField::getString() const
    {
        return string;
    }

    const sf::Font& TextField::getFont() const
    {
        return font;
    }

    unsigned int TextField::getCharacterSize() const
    {
        return character_size;
    }

    float TextField::getLineSpacing() const
    {
        return line_spacing_factor;
    }
    float TextField::getParagraphSpacing() const
    {
        return paragraph_spacing_factor;
    }
    const sf::Color& TextField::getTextColor() const
    {
        return text_color;
    }
    TextField::OverflowType TextField::getOverflowType() const
    {
        return overflow_type;
    }
    bool TextField::isWrapping() const
    {
        return wrapping;
    }
    TextField::Alignment TextField::getHorizontalAlignment() const
    {
        return horizontal_alignment;
    }
    TextField::Alignment TextField::getVerticalAlignment() const
    {
        return vertical_alignment;
    }

    void TextField::setSelection(int p_start, int p_end)
    {
        if(start_selection != p_start || end_selection != p_end)
        {
            start_selection = p_start;
            end_selection = p_end;
            geometry_need_update = true;
        }
    }

    void TextField::setSelectionColor(const sf::Color& p_color)
    {
        if(selection_color != p_color)
        {
            selection_color = p_color;

            if (!geometry_need_update)
            {
                for (int i = 0; i < selection_vertices.getVertexCount(); i++)
                    selection_vertices[i].color = selection_color;
            }
        }
    }
    
    const sf::Color& TextField::getSelectionColor() const
    {
        return selection_color;
    }

    sf::Vector2f TextField::findCharacterPos(int p_index) const
    {
        ensureGeometryUpdate();

        int current_index = 0;

        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(current_index == p_index) 
                {
                    return text_transform.transformPoint(sf::Vector2f(text_info[i][j].position.x, text_info[i][j].position.y - character_size));
                }
                current_index++;
            }
        }

        // The text is empty or index is out of bounds, just return the position of the first/last character.
        if(current_index == 0 || p_index < 0) return text_transform.transformPoint(sf::Vector2f(start_of_string.x, start_of_string.y - character_size));
        return text_transform.transformPoint(sf::Vector2f(end_of_string.x, end_of_string.y - character_size));
    }

    int TextField::findIndex(const sf::Vector2f& p_position) const
    {
        ensureGeometryUpdate();

        float smallest_distance = 0.f;
        int smallest_distance_index = 0; 
        int current_index = 0; 
        bool first = true;

        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                sf::Vector2f distance_vector = text_transform.transformPoint(text_info[i][j].position) - p_position;
                float square_distance = distance_vector.x * distance_vector.x + distance_vector.y * distance_vector.y; // No need to pull the square root. 

                if(first)
                {
                    smallest_distance = square_distance;
                    smallest_distance_index = current_index;
                    first = false;
                }

                if(square_distance < smallest_distance)
                {
                    smallest_distance = square_distance;
                    smallest_distance_index = current_index;
                }
                current_index++;
            }
        }
        // The text is empty.
        if(current_index == 0) return 0;

        // Since the cursor should appear behind the last character and not in front, we need to also check that position.
        sf::Vector2f distance_vector = text_transform.transformPoint(end_of_string) - p_position;
        float square_distance = distance_vector.x * distance_vector.x + distance_vector.y * distance_vector.y;
        if(square_distance < smallest_distance)
        {
            smallest_distance = square_distance;
            smallest_distance_index = current_index;
        }

        return smallest_distance_index;
    }

    void TextField::ensureGeometryUpdate() const
    {
        // Ensure an update is needed due to a change of the text.
        if(!geometry_need_update) return;
        // Mark as updated.
        geometry_need_update = false;

        // Update geometry.
        applyStringGeometry(string);

        // Update overflow.
        if(overflow_type != Overflow)
        {
            applyStringGeometry(applyOverflowType());
        }

        // Change alignment.
        applyHorizontalAlignment();
        applyVerticalAlignment();

        // Update selection.
        applySelection();
    }

    void TextField::draw() const 
    {
        Widget::draw();

        ensureGeometryUpdate();

        window.setView(getMask() == nullptr ? canvas.getView() : getMask()->getView()); // Use parent's view if available.

        sf::RenderStates states;
        states.transform *= text_transform;
        states.texture = &font.getTexture(character_size);
        window.draw(vertices, states);

        states.texture = nullptr;
        window.draw(selection_vertices, states);
    }

    void TextField::addGlyphQuad(sf::VertexArray& p_vertices, const sf::Vector2f& p_position, const sf::Color& p_color, const sf::Glyph& p_glyph) const
    {
        // Bounds of each quad.
        float left = p_glyph.bounds.left;
        float top = p_glyph.bounds.top;
        float right = p_glyph.bounds.left + p_glyph.bounds.width;
        float bottom = p_glyph.bounds.top  + p_glyph.bounds.height;

        // UV texture coordinates that match up the texture of the font with the individual quads.
        float u1 = p_glyph.textureRect.left;
        float v1 = p_glyph.textureRect.top;
        float u2 = p_glyph.textureRect.left + p_glyph.textureRect.width;
        float v2 = p_glyph.textureRect.top  + p_glyph.textureRect.height;

        // Every quad is made up from two triangles, therefore 6 vertices are added to the passed vertex array.
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + bottom), p_color, sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + top), p_color,    sf::Vector2f(u1, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + top), p_color,   sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left, p_position.y + bottom), p_color, sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + top), p_color,   sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right, p_position.y + bottom), p_color,sf::Vector2f(u2, v2)));
    }

    void TextField::applyHorizontalAlignment() const
    {
        if(horizontal_alignment == Left) return;
        for (int i = 0; i < text_info.size(); i++)
        {
            // Skip empty lines.
            if(text_info[i].size() == 0) continue;

            sf::Vector2f start_position = vertices[text_info[i][0].start_vertex].position;
            sf::Vector2f end_position = vertices[text_info[i][text_info[i].size() - 1].end_vertex].position;
            float distanceToMove = 0.f;
            if(horizontal_alignment == Right)
            {
                distanceToMove = getSize().x - end_position.x;
            }
            else if(horizontal_alignment == Centered)
            {
                distanceToMove = (getSize().x/2.f) - (end_position.x - start_position.x)/2.f;
            }

            // Update actual vertices.
            for (int j = text_info[i][0].start_vertex; j < text_info[i][text_info[i].size() - 1].end_vertex + 1; j++)
            {
                vertices[j].position.x += (int)distanceToMove;
            }

            // Update text_info array.
            for (int j = 0; j < text_info[i].size(); j++)
            {
                text_info[i][j].position.x += (int)distanceToMove;
            }
            start_of_string.x += (int)distanceToMove;
            end_of_string.x += (int)distanceToMove;
        }
    }

    void TextField::applyVerticalAlignment() const
    {
        float distanceToMove = 0.f;

        if(vertical_alignment == Top) return;
        else if(vertical_alignment == Middle)
            distanceToMove = (getSize().y/2.f) - (text_bounds.height/2.f);
        else if (vertical_alignment == Bottom)
            distanceToMove = getSize().y - text_bounds.height;
        
        // Update actual vertices.
        for (int i = 0; i < vertices.getVertexCount(); i++)
        {
            vertices[i].position.y += (int)distanceToMove;
        }
        // Update text_info array.
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                text_info[i][j].position.y += (int)distanceToMove;
            }
        }
        
        start_of_string.y += vertices.getVertexCount() == 0 ? (int)distanceToMove - character_size/2.f : (int)distanceToMove;
        end_of_string.y += (int)distanceToMove;
    }

    sf::String TextField::applyOverflowType() const
    {
        sf::String modified_string = string;
        int current_index = 0;
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(vertices[text_info[i][j].end_vertex].position.x > getSize().x || vertices[text_info[i][j].start_vertex].position.y >= getSize().y)
                {
                    if(overflow_type == Truncate)
                    {
                        return modified_string.substring(0, current_index);
                    }
                    else if (overflow_type == Dotted)
                    {
                        return modified_string.substring(0, current_index - 4 >= 0 ? current_index - 4 : 0) + "...";
                    }
                }
                current_index++;
            }
        }
        return modified_string;
    }

    void TextField::applySelection() const
    {
        selection_vertices.clear();
        int current_index = 0;

        // Find highest character position.
        float highest_character_position = 0.f;
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                highest_character_position = std::max(highest_character_position, vertices[text_info[i][j].start_vertex].position.y);
            }
        }

        // Place vertices for selection.
        for (int i = 0; i < text_info.size(); i++)
        {
            for (int j = 0; j < text_info[i].size(); j++)
            {
                if(current_index >= start_selection && current_index < end_selection)
                {
                    sf::Vector2f position = sf::Vector2f(vertices[text_info[i][j].start_vertex].position.x, highest_character_position);
                    sf::Vector2f size = sf::Vector2f(0.f, highest_character_position - (text_info[i][j].position.y - character_size));
                    if(j + 1 < text_info[i].size())
                    {
                        size.x = vertices[text_info[i][j + 1].start_vertex].position.x - vertices[text_info[i][j].start_vertex].position.x;
                    }
                    else
                    {
                        size.x = end_of_string.x - vertices[text_info[i][j].start_vertex].position.x;
                    }

                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y - size.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x + size.x, position.y - size.y), selection_color));
                    selection_vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y - size.y), selection_color));
                }

                current_index++;
            }
        }
    }

    void TextField::applyStringGeometry(const sf::String& p_string) const
    {
        // Reset and compute important values.
        text_bounds = sf::FloatRect();
        vertices.clear();
        text_info.clear();
        std::vector<CharacterInfo> current_line;

        float line_spacing = font.getLineSpacing(character_size) * line_spacing_factor;
        float paragraph_spacing = font.getLineSpacing(character_size) * paragraph_spacing_factor;
        float x = 0.f;
        float y = character_size;
        start_of_string = sf::Vector2f(x, y);

        // Loop through each character in the string and place its vertices.
        for (int i = 0; i < p_string.getSize(); i++)
        {
            // Store character data.
            sf::Uint32 current_char = p_string[i];
            const sf::Glyph& current_glyph = font.getGlyph(current_char, character_size, false);

            // Place vertices.
            if(current_char == '\n')
            {
                current_line.push_back( { sf::Vector2f(x, y),  (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, font.getGlyph(' ', character_size, false));

                x = 0.f;
                y += paragraph_spacing;
                text_info.push_back(current_line);
                current_line.clear();
            }
            else if(x + current_glyph.advance > getSize().x && wrapping) // Sometimes there will be no space on a line that is too long, so just break the line if it gets too long.
            {
                x = 0.f;
                y += line_spacing;

                text_info.push_back(current_line);
                current_line.clear();

                current_line.push_back( { sf::Vector2f(x, y), (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;
            }
            else if(current_char == ' ' && wrapping) // If there is a space search the next space coming up in the string, and calculate if the distance between both string positions still fits on the current line.
            {
                int result_index = i;
                bool searching = true;
                while(searching)
                {
                    result_index++;
                    
                    if(result_index >= p_string.getSize())
                    {
                        searching = false;
                    }
                    if(p_string[result_index] == ' ')
                    {
                        searching = false;
                    }
                }
                float distanceToBounds = getSize().x - (x + current_glyph.advance);
                float distanceOfChars = 0.f;
                for (int j = i; j < result_index; j++)
                {
                    distanceOfChars += font.getGlyph(p_string[j], character_size, false).advance;
                }

                current_line.push_back( { sf::Vector2f(x, y), (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;

                if(distanceOfChars > distanceToBounds) // We need to break the line.
                {
                    x = 0.f;
                    y += line_spacing;
                    text_info.push_back(current_line);
                    current_line.clear();
                }
            }
            else
            {
                current_line.push_back( { sf::Vector2f(x, y), (int)vertices.getVertexCount(), (int)vertices.getVertexCount() + 5 });
                addGlyphQuad(vertices, sf::Vector2f(x, y), text_color, current_glyph);
                x += current_glyph.advance;
            }

            // Update text_bounds.
            text_bounds.width = std::max(text_bounds.width, x + current_glyph.bounds.left + current_glyph.bounds.width);
            text_bounds.height = std::max(text_bounds.height, y + current_glyph.bounds.top + current_glyph.bounds.height);
        }
        text_info.push_back(current_line);
        end_of_string = sf::Vector2f(x, y);
    }

    Button::Button(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), text(p_canvas, p_font)
    {
        setSize(sf::Vector2f(300.f, 40.f));
        text.setSize(sf::Vector2f(300.f, 40.f));
        text.setFillColor(sf::Color::Transparent);
        text.setString("New Button");
        text.setCharacterSize(15);
        text.setSelectable(false);
        text.setMask(this);
        text.setVerticalAlignment(jui::TextField::Middle);

        setOutlineColor(sf::Color(170, 170, 170));
        setOutlineThickness(1.f);

        f_event = [](){};
    }

    void Button::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        text.setPosition(p_position); 
    }

    void Button::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        text.setSize(p_size);
    }

    void Button::updateLogic()
    {
        if(canvas.getHovered() == this)
        {
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                setFillColor(pressed_color);
                
            }
            else
            {
                setFillColor(highlight_color);
            }
        }
        else
        {
            setFillColor(standard_color);
        }
    }

    void Button::updateEvents(sf::Event& p_event)
    {
        if(canvas.getSelected() == this)
        {
            if(p_event.type == sf::Event::MouseButtonReleased)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    f_event();
                }
            }
        }
    }

    void Button::setOnClickEvent(const std::function<void()>& p_event)
    {
        f_event = p_event;
    }

    void Button::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        text.setLayer(p_layer);
    }
    void Button::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        text.setVisible(p_state);
    }
    void Button::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        text.setUpdated(p_state);
    }
    void Button::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        text.setSelectable(p_state);
    }

    ScrollBar::ScrollBar(Canvas& p_canvas, sf::Font& p_font, Orientation p_orientation) 
        : Widget::Widget(p_canvas), move_up_button(p_canvas, p_font), move_down_button(p_canvas, p_font), scroll_handle(p_canvas), orientation(p_orientation)
    {
        move_up_button.standard_color = sf::Color(241, 241, 241);
        move_up_button.highlight_color = sf::Color(200, 200, 200);
        move_down_button.standard_color = sf::Color(241, 241, 241);
        move_down_button.highlight_color = sf::Color(200, 200, 200);
        move_up_button.setOutlineThickness(0.f);
        move_down_button.setOutlineThickness(0.f);
        setFillColor(sf::Color(241, 241, 241));
        scroll_handle.setFillColor(handle_color);

        move_up_button.setMask(this);
        move_down_button.setMask(this);
        move_up_button.text.setHorizontalAlignment(jui::TextField::Centered);
        move_down_button.text.setHorizontalAlignment(jui::TextField::Centered);
        move_up_button.text.setVerticalAlignment(jui::TextField::Middle);
        move_down_button.text.setVerticalAlignment(jui::TextField::Middle);

        move_up_button.setOnClickEvent([&]()
        {
            setScrollState(state.scroll_handle_position - 10.f, state.scroll_handle_size, state.scroll_list_length);
        });
        move_down_button.setOnClickEvent([&]()
        {
            setScrollState(state.scroll_handle_position + 10.f, state.scroll_handle_size, state.scroll_list_length);
        });

        if(orientation == Vertical)
        {
            setSize(sf::Vector2f(20.f, 200.f));
        }
        else if (orientation == Horizontal)
        {
            setSize(sf::Vector2f(200.f, 20.f));
        }

        setScrollState(0.f, 100.f, 200.f);
        refreshIcons();
    }

    void ScrollBar::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        refreshPosition();
    }

    void ScrollBar::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        refreshSize();
        refreshPosition();
        refreshIcons();
    }
    
    void ScrollBar::setScrollState(float p_position, float p_size, float p_list_length)
    {
        state.scroll_handle_size = p_size > p_list_length ? p_list_length : p_size;
        if(p_position < 0.f)
        {
            state.scroll_handle_position = 0.f;
        }
        else if(p_position + p_size > p_list_length)
        {
            state.scroll_handle_position = p_list_length - p_size;
        }
        else 
        {
            state.scroll_handle_position = p_position;
        }
        state.scroll_list_length = p_list_length;

        float size_ratio = state.scroll_list_length <= state.scroll_handle_size ? 1 : (state.scroll_handle_size / state.scroll_list_length);
        float position_ratio = state.scroll_list_length <= state.scroll_handle_size ? 0 : (state.scroll_handle_position / state.scroll_list_length);

        if(orientation == Vertical)
        {
            float scroll_bar_size = (getSize().y - move_down_button.getSize().y * 2);
            scroll_handle.setSize(sf::Vector2f(getSize().x - handle_margin * 2.f, size_ratio * scroll_bar_size));
            scroll_handle.setPosition(sf::Vector2f(getPosition().x + handle_margin, getPosition().y + move_down_button.getSize().y + position_ratio * scroll_bar_size));
        }
        else if (orientation == Horizontal)
        {
            float scroll_bar_size = (getSize().x - move_down_button.getSize().x * 2);
            scroll_handle.setSize(sf::Vector2f(size_ratio * scroll_bar_size, getSize().y - handle_margin * 2.f));
            scroll_handle.setPosition(sf::Vector2f(getPosition().x + move_down_button.getSize().x + position_ratio * scroll_bar_size, getPosition().y + handle_margin));
        }
        f_onScroll();
    }
    const ScrollBar::ScrollState& ScrollBar::getScrollState()
    {
        return state;
    }

    void ScrollBar::setOnScrollListener(const std::function<void()>& p_event)
    {
        f_onScroll = p_event;
    }

    void ScrollBar::refreshIcons()
    {
        if(orientation == Vertical)
        {
            move_up_button.text.setString(L"\u25b2");
            move_down_button.text.setString(L"\u25bc");
        }
        else if (orientation == Horizontal)
        {
            move_up_button.text.setString(L"\u25c4");
            move_down_button.text.setString(L"\u25ba");
        }
    }
    
    void ScrollBar::refreshSize()
    {
        if(orientation == Vertical)
        {
            move_up_button.setSize(sf::Vector2f(getSize().x, getSize().x)); 
            move_down_button.setSize(sf::Vector2f(getSize().x, getSize().x));
        }
        else if (orientation == Horizontal)
        {
            move_up_button.setSize(sf::Vector2f(getSize().y, getSize().y)); 
            move_down_button.setSize(sf::Vector2f(getSize().y, getSize().y));
        }
    }

    void ScrollBar::refreshPosition()
    {
        move_up_button.setPosition(getPosition());
        if(orientation == Vertical)
        {
            move_down_button.setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y - getSize().x));
        }
        else if (orientation == Horizontal)
        {
            move_down_button.setPosition(sf::Vector2f(getPosition().x + getSize().x - getSize().y, getPosition().y));
        }
        setScrollState(state.scroll_handle_position, state.scroll_handle_size, state.scroll_list_length);
    }

    void ScrollBar::updateEvents(sf::Event& p_event) 
    {
        // Deselect the handle when the left mouse is released.
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left && sf::FloatRect(scroll_handle.getPosition(), scroll_handle.getSize()).contains(getMousePosition(window, &view)))
            {
                handle_selected = true;
            }
        }
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                handle_selected = false;
            }
        }
    }

    void ScrollBar::updateLogic()
    {
        if(canvas.getSelected() == &scroll_handle)
        {
            // Move the handle by calculating the difference between the current and last mouse position.
            if(handle_selected)
            {
                sf::Vector2f difference = getMousePosition(window, &view) - last_mouse_position;
                if(orientation == Vertical)
                {
                    setScrollState(state.scroll_handle_position + (difference.y / getSize().y) * state.scroll_list_length, state.scroll_handle_size, state.scroll_list_length);
                }
                else if (orientation == Horizontal)
                {
                    setScrollState(state.scroll_handle_position + (difference.x / getSize().x) * state.scroll_list_length, state.scroll_handle_size, state.scroll_list_length);
                }
                last_mouse_position = getMousePosition(window, &view);
            }
        }

        // Highlight handle when hovering over it.
        if(sf::FloatRect(scroll_handle.getPosition(), scroll_handle.getSize()).contains(getMousePosition(window, &view)))
        {
            scroll_handle.setFillColor(hovered_handle_color);
        }
        else
        {
            scroll_handle.setFillColor(handle_color);
        }

        // Update last mouse position to prevent the handle from making jumps when selecting it.
        if(!handle_selected)
        {
            last_mouse_position = getMousePosition(window, &view);
        }
    }

    void ScrollBar::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        move_up_button.setLayer(p_layer);
        move_down_button.setLayer(p_layer);
        scroll_handle.setLayer(p_layer);
    }
    void ScrollBar::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        move_up_button.setVisible(p_state);
        move_down_button.setVisible(p_state);
        scroll_handle.setVisible(p_state);
    }
    void ScrollBar::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        move_up_button.setUpdated(p_state);
        move_down_button.setUpdated(p_state);
        scroll_handle.setUpdated(p_state);
    }
    void ScrollBar::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        move_up_button.setSelectable(p_state);
        move_down_button.setSelectable(p_state);
        scroll_handle.setSelectable(p_state);
    }

    LayoutGroup::LayoutGroup(Canvas& p_canvas) 
        : Widget::Widget(p_canvas) 
    {

    }

    void LayoutGroup::setPosition(const sf::Vector2f& p_position)
    {
        sf::Vector2f difference = p_position - Widget::getPosition();
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->move(difference);
        }
        
        Widget::setPosition(p_position);
    }

    sf::Vector2f LayoutGroup::getGroupSize() const
    {
        if(widget_list.size() < 1) return sf::Vector2f();
        sf::Vector2f smallest_position = widget_list[0]->getPosition();
        sf::Vector2f biggest_position = widget_list[0]->getPosition() + widget_list[0]->getSize();
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->getPosition().x < smallest_position.x || widget_list[i]->getPosition().y < smallest_position.y)
            {
                smallest_position = widget_list[i]->getPosition();
            }
            sf::Vector2f current_biggest_position = widget_list[i]->getPosition() + widget_list[i]->getSize();
            if(current_biggest_position.x > biggest_position.x || current_biggest_position.y > biggest_position.y)
            {
                biggest_position = current_biggest_position;
            }
        }
        return biggest_position - smallest_position;
    }

    void LayoutGroup::listWidgets(Orientation orientation, float p_spacing)
    {
        float current_height;
        if(orientation == Vertical)
        {
            current_height = Widget::getPosition().y;
            for (int i = 0; i < widget_list.size(); i++)
            {
                widget_list[i]->setPosition(sf::Vector2f(Widget::getPosition().x, current_height));
                current_height += widget_list[i]->getSize().y + p_spacing;
            }
        }
        else if (orientation == Horizontal)
        {
            current_height = Widget::getPosition().x;
            for (int i = 0; i < widget_list.size(); i++)
            {
                widget_list[i]->setPosition(sf::Vector2f(current_height, Widget::getPosition().y));
                current_height += widget_list[i]->getSize().x + p_spacing;
            }
        }
    }

    void LayoutGroup::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->setMask(p_widget);
        }
    }

    void LayoutGroup::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->setVisible(p_state);
        }
    }
    void LayoutGroup::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->setUpdated(p_state);
        }
    }
    void LayoutGroup::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->setSelectable(p_state);
        }
    }
    void LayoutGroup::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        for (int i = 0; i < widget_list.size(); i++)
        {
            widget_list[i]->setLayer(p_layer);
        }
    }

    void LayoutGroup::add(Widget* p_widget)
    {
        widget_list.push_back(p_widget);
    }

    void LayoutGroup::remove(Widget* p_widget)
    {
        for(int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i] == p_widget)
            {
                widget_list.erase(widget_list.begin() + i);
                return;
            }
        }
    }

    Widget* LayoutGroup::get(int p_index)
    {
        if(p_index < 0 || p_index >= widget_list.size()) return nullptr;
        return widget_list[p_index];
    }

    int LayoutGroup::getCount()
    {
        return widget_list.size();
    }

    ScrollList::ScrollList(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), scroll_bar(p_canvas, p_font, Orientation::Vertical), content(p_canvas)
    {
        setSize(sf::Vector2f(200.f, 300.f));
        setOutlineColor(sf::Color(170, 170, 170));
        setOutlineThickness(1.f);

        content.setOutlineThickness(0.f);
        content.setFillColor(sf::Color::Transparent);
        content.setSelectable(false);
        scroll_bar.setMask(this);

        scroll_bar.setOnScrollListener([&]()
        {
            float starting_position;
            if(scroll_bar.getScrollState().scroll_list_length <= scroll_bar.getScrollState().scroll_handle_size) 
            {
                starting_position = 0;
            }
            else 
            {
                starting_position = (scroll_bar.getScrollState().scroll_handle_position / scroll_bar.getScrollState().scroll_list_length) * content.getGroupSize().y;
            } 
            content.setPosition(sf::Vector2f(getPosition().x, getPosition().y - starting_position));
        });
    }

    void ScrollList::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        scroll_bar.setPosition(sf::Vector2f(getPosition().x + getSize().x - scrollbar_width, getPosition().y));
    }

    void ScrollList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        scroll_bar.setPosition(sf::Vector2f(getPosition().x + getSize().x - scrollbar_width, getPosition().y));
        scroll_bar.setSize(sf::Vector2f(scrollbar_width, getSize().y));
    }

    void ScrollList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        content.setLayer(p_layer);
        scroll_bar.setLayer(p_layer);
    }
    void ScrollList::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        scroll_bar.setVisible(p_state);
        content.setVisible(p_state);
    }
    void ScrollList::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        scroll_bar.setUpdated(p_state);
        content.setUpdated(p_state);
    }
    void ScrollList::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        scroll_bar.setSelectable(p_state);
        content.setSelectable(p_state);
    }

    void ScrollList::add(Widget* p_widget)
    {
        content.add(p_widget);
        p_widget->setMask(this);
        setLayer(getLayer());
        scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position, getSize().y, content.getGroupSize().y);
    }

    void ScrollList::remove(Widget* p_widget)
    {
        content.remove(p_widget);
        p_widget->setMask(nullptr);
        scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position, getSize().y, content.getGroupSize().y);
    }

    Widget* ScrollList::get(int p_index)
    {
        return content.get(p_index);
    }

    int ScrollList::getCount()
    {
        return content.getCount();
    }

    void ScrollList::setScrollBarWidth(float p_width)
    {
        scrollbar_width = p_width;
        setSize(getSize());
    }
    float ScrollList::getScrollBarWidth()
    {
        return scrollbar_width;
    }

    void ScrollList::listWidgets(Orientation orientation)
    {
        content.listWidgets(orientation);
    }

    void ScrollList::updateEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseWheelScrolled)
        {
            // ScrollList should only be scrollable if it or its content is hovered. An exception are ScrollLists inside ScrollLists. 
            // When hovering over the child, the parent ScrollList should not be scrollable. dynamic_cast is used to check whether the underlying widget is in fact a ScrollList.
            Widget* hovered_widget = canvas.getHovered();
            bool scrollable = hovered_widget == this;
            if(!scrollable)
            {
                for (int i = 0; i < content.getCount(); i++)
                {
                    if(dynamic_cast<ScrollList*>(content.get(i)) != nullptr) continue;
                    scrollable = hovered_widget == content.get(i);
                    if(scrollable) break;
                }
            }

            if(scrollable)
            {
                scroll_bar.setScrollState(scroll_bar.getScrollState().scroll_handle_position - (10.f * p_event.mouseWheelScroll.delta), getSize().y, content.getGroupSize().y);
            }
        }
    }

    DropDownList::DropDownList(Canvas& p_canvas, sf::Font& p_font)
        : font(p_font), Widget::Widget(p_canvas), dropdown_button(p_canvas, p_font), scrolllist(p_canvas, p_font)
    {
        scrolllist.setVisible(false);
        setPosition(sf::Vector2f(0.f, 0.f));
        setSize(sf::Vector2f(300.f, 40.f));
        scrolllist.setFillColor(sf::Color(200, 200, 200, 200));

        dropdown_button.text.setString("New DropDownList");
        dropdown_button.setOnClickEvent([&]()
        {
            if(!scrolllist.isVisible())
                f_onSelect();
            scrolllist.setVisible(true);
        });

        f_onSelect = [](){};
        f_onDeselect = [](){};
    }

    void DropDownList::setPosition(const sf::Vector2f& p_position)
    {
        dropdown_button.setPosition(p_position);
        Widget::setPosition(p_position);
        scrolllist.setPosition(sf::Vector2f(p_position.x, p_position.y + dropdown_button.getSize().y));
    }
    void DropDownList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        dropdown_button.setSize(p_size);
        scrolllist.setSize(sf::Vector2f(p_size.x, 200.f));

        // Resize all buttons.
        for (int i = 0; i < scrolllist.getCount(); i++)
        {
            scrolllist.get(i)->setSize(sf::Vector2f(p_size.x - scrolllist.getScrollBarWidth(), scrolllist.get(i)->getSize().y));
        }
        scrolllist.listWidgets(jui::Vertical);
    }

    void DropDownList::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(!scrolllist.mouseOver() && !dropdown_button.mouseOver())
                {
                    if(scrolllist.isVisible())
                        f_onDeselect();
                    scrolllist.setVisible(false);
                }
            }
        }
    }

    void DropDownList::setMask(Widget* p_widget)
    {
        Widget::setMask(p_widget);
        dropdown_button.setMask(p_widget);
        scrolllist.setMask(p_widget);
    }
    void DropDownList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        dropdown_button.setLayer(p_layer);
        scrolllist.setLayer(p_layer);
    }
    void DropDownList::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        dropdown_button.setVisible(p_state);
        if(!p_state)
            scrolllist.setVisible(p_state);
    }
    void DropDownList::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        dropdown_button.setUpdated(p_state);
        scrolllist.setUpdated(p_state);
    }
    void DropDownList::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        dropdown_button.setSelectable(p_state);
        scrolllist.setSelectable(p_state);
    }

    void DropDownList::setOnSelectEvent(const std::function<void()>& p_event)
    {
        f_onSelect = p_event;
    }
    void DropDownList::setOnDeselectEvent(const std::function<void()>& p_event)
    {
        f_onDeselect = p_event;
    }

    void DropDownList::setButtonColor(const sf::Color& p_color)
    {
        dropdown_button.standard_color = p_color;
    }    
    void DropDownList::setButtonTextColor(const sf::Color& p_color)
    {
        dropdown_button.text.setTextColor(p_color);
    }

    void DropDownList::setString(const sf::String& p_string)
    {
        dropdown_button.text.setString(p_string);
    }

    sf::String DropDownList::getString()
    {
        return dropdown_button.text.getString();
    }

    void DropDownList::setList(const std::vector<sf::String>& p_elements)
    {
        for (int i = 0; i < scrolllist.getCount(); i++)
        {
            Widget* current_widget = scrolllist.get(i); 
            scrolllist.remove(current_widget);
            delete current_widget;
        }
        
        for (int i = 0; i < p_elements.size(); i++)
        {
            Button* button = new Button(canvas, font);
            button->setSize(sf::Vector2f(dropdown_button.getSize().x - 20.f, 40.f));
            button->text.setString(p_elements[i]);
            button->standard_color = sf::Color(240, 240, 240);

            // The button click listener gets a copy of a pointer to this DropDownList, as well as a pointer to itself.
            button->setOnClickEvent([this, button]()
            {
                scrolllist.setVisible(false);
                dropdown_button.text.setString(button->text.getString());
                f_onDeselect();
            });
 
            scrolllist.add(button);
        }
        scrolllist.listWidgets(Orientation::Vertical);
        scrolllist.setVisible(false);
    }

    void DropDownList::setListLength(float p_length)
    {
        scrolllist.setSize(sf::Vector2f(dropdown_button.getSize().x, p_length));
    }

    InputField::InputField(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), text(p_canvas, p_font), placeholder_text(p_canvas, p_font), cursor(p_canvas)
    {
        setPosition(sf::Vector2f(0.f, 0.f));
        setSize(sf::Vector2f(300.f, 40.f));
        setCharacterSize(15);
        setOutlineColor(sf::Color(170, 170, 170));
        setOutlineThickness(1.f);

        text.setTextColor(sf::Color::Black);
        text.setFillColor(sf::Color::Transparent);
        text.setVerticalAlignment(TextField::Alignment::Middle);
        text.setString("");
        text.enableWrapping(false);
        text.setOutlineThickness(0.f);
        text.setMask(this);
        placeholder_text.setTextColor(placeholder_text_color);
        placeholder_text.setFillColor(sf::Color::Transparent);
        placeholder_text.setVerticalAlignment(TextField::Alignment::Middle);
        placeholder_text.setString("Type here...");
        placeholder_text.setOutlineThickness(0.f);
        placeholder_text.enableWrapping(true);
        placeholder_text.setVisible(false);
        cursor.setFillColor(sf::Color::Black);
        cursor.setOutlineThickness(0.f);
        cursor.setMask(this);
        cursor.setVisible(false);

        refreshCursor();
 
        f_customIsCharValid = [](sf::Uint32 p_char){ return true; };
        f_onSelect = [](){};
        f_onDeselect = [](){};
    }

    void InputField::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        text.setPosition(p_position);
        placeholder_text.setPosition(sf::Vector2f(p_position.x + text_padding, p_position.y));
        refreshCursor();
    }

    void InputField::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        text.setSize(p_size);
        placeholder_text.setSize(p_size);
    }

    void InputField::setString(const sf::String& p_string)
    {
        text.setString(p_string);
        cursor_index = p_string.getSize();
        drag_cursor_index = cursor_index;
        refreshCursor();
    }

    const sf::String& InputField::getString()
    {
        return text.getString();
    }

    void InputField::setPlaceholderText(const sf::String& p_string)
    {
        placeholder_text.setString(p_string);
    }

    void InputField::setCharacterSize(unsigned int p_size)
    {
        text.setCharacterSize(p_size);
        placeholder_text.setCharacterSize(p_size);
        cursor.setSize(sf::Vector2f(1.f, p_size));
    }

    void InputField::setInputType(sf::Uint32 p_type)
    {
        type = p_type;
    }

    void InputField::setCharacterLimit(int p_min, int p_max)
    {
        max_character_limit = p_max;
        min_character_limit = p_min;
    }

    void InputField::setValueLimit(float p_min, float p_max)
    {
        max_value_limit = p_max;
        min_value_limit = p_min;
    }

    bool InputField::isInputValid()
    {
        return !(limit_error || input_error || date_error);
    }

    void InputField::setValidChars(const std::function<bool(sf::Uint32 p_char)>& p_function)
    {
        f_customIsCharValid = p_function;
    }

    void InputField::setOnSelectEvent(const std::function<void()>& p_event)
    {
        f_onSelect = p_event;
    }
    void InputField::setOnDeselectEvent(const std::function<void()>& p_event)
    {
        f_onDeselect = p_event;
    }

    bool InputField::isSelected() const
    {
        return canvas.getSelected() == this || canvas.getSelected() == &text || canvas.getSelected() == &placeholder_text || canvas.getSelected() == &cursor;
    }

    bool InputField::isHovered() const
    {
        return canvas.getHovered() == this || canvas.getHovered() == &text || canvas.getHovered() == &placeholder_text || canvas.getHovered() == &cursor;
    }

    void InputField::updateLogic()
    {
        if(!isSelected())
        {
            if(isHovered())
            {
                setFillColor(highlight_color);
            }
            else
            {
                if(isInputValid())
                {
                    setFillColor(standard_color);
                }
                else
                {
                    setFillColor(error_color);
                }
            }
        }
        else
        {
            if(isInputValid())
            {
                setFillColor(standard_color);
            }
            else
            {
                setFillColor(error_color);
            }
        }

        // Make the text cursor blink.
        if(isSelected())
        {
            blinking_timer += Canvas::getDeltaTime();
            if(blinking_timer >= blinking_interval)
            {
                cursor.setVisible(!cursor.isVisible());
                blinking_timer = 0.f;
            }
        }
        else
        {
            cursor.setVisible(false);
        }

        // Update visibility of the placeholder text.
        placeholder_text.setVisible(text.getString().getSize() == 0);

        if(is_making_mouse_selection)
        {
            drag_cursor_index = text.findIndex(getMousePosition(window, &view));
            moveTextHorizontally(drag_cursor_index);

            cursor.setPosition(text.findCharacterPos(cursor_index));
            text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
        }
    }

    void InputField::updateEvents(sf::Event& p_event)
    {
        if(isSelected())
        {
            if(p_event.type == sf::Event::KeyPressed)
            {
                if(p_event.key.code == sf::Keyboard::Left)
                {
                    cursor.setVisible(true);
                    blinking_timer = 0.f;

                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index--;
                        refreshCursor();
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else if(cursor_index != drag_cursor_index) // If we are not making a shift selection, but something is selected.
                    {
                        is_making_mouse_selection = false;
                        cursor_index = std::min(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else // If we are not making any selection.
                    {
                        cursor_index--;
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                }
                if(p_event.key.code == sf::Keyboard::Right)
                {
                    cursor.setVisible(true);
                    blinking_timer = 0.f;

                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index++;
                        refreshCursor();
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else if(cursor_index != drag_cursor_index)
                    {
                        is_making_mouse_selection = false;
                        cursor_index = std::max(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                        text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    }
                    else
                    {
                        cursor_index++;
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                }
                if(p_event.key.code == sf::Keyboard::LShift || p_event.key.code == sf::Keyboard::RShift)
                {
                    is_making_shift_selection = true;
                }
            }
            if(p_event.type == sf::Event::KeyReleased)
            {
                input_error = false;
                if(p_event.key.code == sf::Keyboard::LShift || p_event.key.code == sf::Keyboard::RShift)
                {
                    is_making_shift_selection = false;
                }
            }

            // Change cursor index with mouse click and mouse selection.
            if(p_event.type == sf::Event::MouseButtonPressed)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    cursor_index = text.findIndex(getMousePosition(window, &view));
                    refreshCursor();
                    drag_cursor_index = cursor_index;
                    is_making_mouse_selection = true;
                }
            }
            if(p_event.type == sf::Event::MouseButtonReleased)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    is_making_mouse_selection = false;
                }
            }

            // Handle inputted text.
            if(p_event.type == sf::Event::TextEntered)
            {
                sf::Uint32 input = p_event.text.unicode;

                cursor.setVisible(true);
                blinking_timer = 0.f;

                // Crtl + C (3)
                if(input == 3)
                {
                    // If the user made a selection.
                    if(drag_cursor_index != cursor_index)
                    {
                        sf::Clipboard::setString(text.getString().substring(std::min(cursor_index, drag_cursor_index), std::abs(cursor_index - drag_cursor_index)));
                    }
                    return;
                }

                // Crtl + A (1)
                if(input == 1)
                {
                    drag_cursor_index = 0;
                    cursor_index = text.getString().getSize();
                    refreshCursor();
                    text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                    return;
                }

                // On any press.
                bool made_selection = cursor_index != drag_cursor_index;
                if(made_selection)
                {
                    sf::String modified_string = text.getString();
                    modified_string.erase(std::min(cursor_index, drag_cursor_index), std::abs(cursor_index - drag_cursor_index));
                    cursor_index = std::min(cursor_index, drag_cursor_index);
                    drag_cursor_index = cursor_index;
                    text.setString(modified_string);
                    refreshCursor();
                    text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));
                }

                // Crtl + V (22)
                if(input == 22)
                {
                    sf::String toInsert = sf::Clipboard::getString();
                    // Check if string is not to big.
                    if(toInsert.getSize() + text.getString().getSize() > max_character_limit) 
                    {
                        input_error = true;
                        return;
                    }
                    // Check whether string contains undesired characters.
                    for (int i = 0; i < toInsert.getSize(); i++)
                    {
                        if(!isCharValid(toInsert[i])) 
                        {
                            input_error = true;
                            return;
                        }
                    }
                    if(type & Capitalized)
                    {
                        toInsert = toUppercase(toInsert);
                    }
                    sf::String modified_string = text.getString();
                    modified_string.insert(cursor_index, toInsert);
                    text.setString(modified_string);
                    cursor_index += toInsert.getSize();
                    drag_cursor_index = cursor_index;
                    refreshCursor();
                    return;
                }

                // On backspace press.
                if(input == L'\b')
                {
                    if(cursor_index > 0 && !made_selection)
                    {
                        sf::String modified_string = text.getString();
                        modified_string.erase(cursor_index - 1);
                        text.setString(modified_string);
                        cursor_index--;
                        drag_cursor_index = cursor_index;
                        refreshCursor();
                    }
                }
                // On valid character input.
                else if(isCharValid(input) && text.getString().getSize() < max_character_limit)
                {
                    sf::String modified_string = text.getString();
                    if(type & Capitalized)
                    {
                        input = toUppercase(input);
                    }
                    modified_string.insert(cursor_index, input);
                    text.setString(modified_string);
                    cursor_index++;
                    refreshCursor();
                    drag_cursor_index = cursor_index;
                }
                else
                {
                    input_error = true;
                }

                // Update character limit error.
                if(text.getString().getSize() < min_character_limit && text.getString().getSize() > 0)
                {
                    limit_error = true;
                }
                else
                {
                    limit_error = false;
                }

                refreshDateFormat();
            }
        }

        // Change mouse cursor.
        if(p_event.type == sf::Event::MouseMoved)
        {

            bool is_currently_hovered = isHovered();

            if(is_currently_hovered && !was_hovered)
            {
                Cursor::setCursor(sf::Cursor::Text, window);
            }
            if(!is_currently_hovered && was_hovered)
            {
                Cursor::setCursor(sf::Cursor::Arrow, window);
            }

            was_hovered = is_currently_hovered;
        }

        // OnSelect and OnDeselect events.
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            bool is_currently_selected = isSelected();
            if(!was_selected && is_currently_selected)
            {
                refreshLimitedFormat();
                f_onSelect();
            }
            if(was_selected && !is_currently_selected)
            {
                refreshLimitedFormat();
                f_onDeselect();

                // Hide selection.
                drag_cursor_index = cursor_index;
                text.setSelection(std::min(drag_cursor_index, cursor_index), std::max(drag_cursor_index, cursor_index));

                // Move text back.
                moveTextHorizontally(0);
            }
            was_selected = is_currently_selected;
        }
    }

    void InputField::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        text.setLayer(p_layer);
        placeholder_text.setLayer(p_layer);
        cursor.setLayer(p_layer);
    }

    void InputField::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        text.setVisible(p_state);
        if(!p_state)
        {
            placeholder_text.setVisible(false);
            cursor.setVisible(false);
        }
    }

    void InputField::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        text.setUpdated(p_state);
        placeholder_text.setUpdated(p_state);
        cursor.setUpdated(p_state);
    }

    void InputField::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        text.setSelectable(p_state);
        placeholder_text.setSelectable(p_state);
        cursor.setSelectable(p_state);
    }

    void InputField::refreshCursor()
    {
        // Out of bounds check.
        if(cursor_index < 0)
        {
            cursor_index = 0;
        }
        if(cursor_index > text.getString().getSize())
        {
            cursor_index = text.getString().getSize();
        }

        // Update text position.
        moveTextHorizontally(cursor_index);

        // Update cursor position.
        cursor.setPosition(text.findCharacterPos(cursor_index));
    }

    bool InputField::isCharValid(sf::Uint32 p_char)
    {
        if(type & Custom) return f_customIsCharValid(p_char);
        if(type == Date) return isdigit(p_char) || p_char == '.';
        if(type & Standard) return true;

        bool value = false;

        if(type & IntegerNumber)
            value |= isdigit(p_char) || p_char == '+' || p_char == '-';
        if(type & DecimalNumber)
            value |= isdigit(p_char) || p_char == ',' || p_char == '.' || p_char == '+' || p_char == '-';
        if(type & HexadecimalNumber)
            value |= isdigit(p_char) || p_char == 'a' || p_char == 'b' || p_char == 'c' || p_char == 'd' || p_char == 'e' || p_char == 'f'
            || p_char == 'A' || p_char == 'B' || p_char == 'C' || p_char == 'D' || p_char == 'E' || p_char == 'F' || p_char == 'x';
        if(type & BinaryNumber)
            value |= p_char == '0' || p_char == '1' || p_char == 'b';
        if(type & Alphanumeric)
            value |= isalnum(p_char);
        if(type & Alpha)
            value |= isalpha(p_char);

        return value;
    }

    void InputField::refreshDateFormat()
    {
        if(type == Date)
        {
            setCharacterLimit(10, 10);
            date_error = text.getString().getSize() == 10 && (text.getString()[2] != '.' || text.getString()[5] != '.');
        }
    }

    void InputField::refreshLimitedFormat()
    {
        if(type & LimitedValue)
        {
            if(type & IntegerNumber)
            {
                text.setString(toString(clampValue(stringToInt(text.getString()), (int)min_value_limit, (int)max_value_limit), 2));
            }
            else if(type & DecimalNumber)
            {
                text.setString(toString(clampValue(stringToFloat(text.getString()), min_value_limit, max_value_limit), 2));
            }
            else if(type & HexadecimalNumber)
            {
                text.setString(toHexadecimalString(clampValue(hexadecimalToInt(text.getString()), (unsigned int)min_value_limit, (unsigned int)max_value_limit)));
            }
            else if(type & BinaryNumber)
            {
                text.setString(toBinaryString(clampValue(binaryToInt(text.getString()), (unsigned int)min_value_limit, (unsigned int)max_value_limit)));
            }
        }
    }

    void InputField::moveTextHorizontally(int p_index)
    {
        // If the cursor goes out of bounds we move the text so it still is inside, also adding a small padding.
        sf::Vector2f index_position = text.findCharacterPos(p_index);
        if(index_position.x < getPosition().x)
        {
            text.move(sf::Vector2f(getPosition().x - index_position.x + text_padding, 0.f));
        }
        else if (index_position.x >= getPosition().x + getSize().x)
        {
            text.move(sf::Vector2f((getPosition().x + getSize().x) - index_position.x - text_padding, 0.f));
        }

        // Makes that if the text is longer than the actual inputfield and you change its size the text will be moved to the right of the inputfield accordingly.
        sf::Vector2f first_position = text.findCharacterPos(0);
        sf::Vector2f last_position = text.findCharacterPos(text.getString().getSize());
        float text_width = last_position.x - first_position.x;
        if(text_width > getSize().x - 2.f * text_padding)
        {
            if(last_position.x < getPosition().x + getSize().x)
            {
                text.move(sf::Vector2f((getPosition().x + getSize().x) - last_position.x - text_padding, 0.f));
            }
        }
        // If the text is shorter just fix it in place with some padding.
        else
        {
            text.setPosition(sf::Vector2f(getPosition().x + text_padding, text.getPosition().y));
        }
    }

    float Canvas::s_delta_time;

    Canvas::Canvas(sf::RenderWindow& p_window)
        : window(p_window)
    {
        view.reset(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y));
        view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    }
    
    void Canvas::drawAll() 
    {
        // Save the current view as copy and assign new one.
        sf::View currentView = window.getView();

        // Draw visible widgets.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isVisible())
            {
                widget_list[i]->draw();
            }
        }

        window.setView(view);

        // Draw here.

        window.setView(currentView);
    }
    
    void Canvas::updateAllLogic() 
    {
        // Updates of all widgets.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isUpdated() && widget_list[i]->isVisible())
            {
                widget_list[i]->updateLogic();
            }
        }

        Canvas::s_delta_time = clock.restart().asSeconds();
    }
    
    void Canvas::updateAllEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {     
                // Set selected widget.
                std::vector<int> widgets_under_mouse;
                for (int i = 0; i < widget_list.size(); i++)
                {
                    if(widget_list[i]->mouseOver() && widget_list[i]->isSelectable() && widget_list[i]->isVisible())
                    {
                        widgets_under_mouse.push_back(i);
                    }
                }
                if(widgets_under_mouse.size() == 0)
                {
                    select(nullptr);
                }
                else
                {
                    int selected_widget_index = widgets_under_mouse[widgets_under_mouse.size() - 1];
                    select(widget_list[selected_widget_index]);
                }
            }
        }
        if(p_event.type == sf::Event::Resized)
        {
            // Reset views.
            for (int i = 0; i < widget_list.size(); i++)
            {
                widget_list[i]->setPosition(widget_list[i]->getPosition());
            }
            view.reset(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y));
            view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        }
        if(p_event.type == sf::Event::MouseMoved)
        {
            std::vector<int> widgets_under_mouse;
            for (int i = 0; i < widget_list.size(); i++)
            {
                if(widget_list[i]->mouseOver() && widget_list[i]->isSelectable() && widget_list[i]->isVisible())
                {
                    widgets_under_mouse.push_back(i);
                }
            }
            hovered_widget = widgets_under_mouse.size() ? widget_list[widgets_under_mouse[widgets_under_mouse.size() - 1]] : nullptr;
        }

        if(p_event.type == sf::Event::KeyPressed)
        {
            if(p_event.key.code == sf::Keyboard::F12)
            {
                print();
            }
        }

        // Update widget events always.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i]->isUpdated() && widget_list[i]->isVisible())
            {
                widget_list[i]->updateEvents(p_event);
            }
        }
    }
    
    void Canvas::add(Widget* p_widget) 
    {
        // Check if widget alreay exists.
        if(find(p_widget) != -1) return;

        // Add widget.
        widget_list.push_back(p_widget);

        moveToLayer(p_widget, p_widget->getLayer());
    }

    int Canvas::find(Widget* p_widget) const
    {
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i] == p_widget)
            {
                return i;
            }
        }
        return -1;
    }

    void Canvas::remove(Widget* p_widget) 
    {
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(widget_list[i] == p_widget)
            {
                widget_list.erase(widget_list.begin() + i);
                return;
            }
        }
    }
    
    void Canvas::moveToLayer(Widget* p_widget, int p_layer)
    {
        // First remove the given widget.
        remove(p_widget);
        // Insert the widget behind the last widget that has the same layer as the given one.
        for (int i = 0; i < widget_list.size(); i++)
        {
            if(p_layer < widget_list[i]->getLayer())
            {
                widget_list.insert(widget_list.begin() + i, p_widget);
                return;
            }
        }
        // If the layer is too high/not present, just insert at the end.
        widget_list.insert(widget_list.begin() + widget_list.size(), p_widget);
    }

    Widget* Canvas::getSelected() 
    {
        return selected_widget;
    }

    Widget* Canvas::getHovered()
    {
        return hovered_widget;
    }
    
    const sf::View& Canvas::getView() const
    {
        return view;
    }
    
    void Canvas::select(Widget* p_widget)
    {
        selected_widget = p_widget;
    }

    void Canvas::print()
    {
        std::cout << "Canvas (" << this << ")" << std::endl;
        for (int i = 0; i < widget_list.size(); i++)
        {
            sf::String type = "Widget";
            TextField* text_field = dynamic_cast<TextField*>(widget_list[i]);
            Button* button = dynamic_cast<Button*>(widget_list[i]);
            ScrollList* scrolllist = dynamic_cast<ScrollList*>(widget_list[i]);
            DropDownList* dropdownlist = dynamic_cast<DropDownList*>(widget_list[i]);
            InputField* inputfield = dynamic_cast<InputField*>(widget_list[i]);
            ScrollBar* scrollbar = dynamic_cast<ScrollBar*>(widget_list[i]);
            LayoutGroup* layoutgroup = dynamic_cast<LayoutGroup*>(widget_list[i]);
            if(text_field != nullptr) type = "TextField (Content: " + text_field->getString() + ")";
            if(button != nullptr) type = "Button";
            if(scrolllist != nullptr) type = "ScrollList";
            if(inputfield != nullptr) type = "InputField";
            if(scrollbar != nullptr) type = "ScrollBar";
            if(layoutgroup != nullptr) type = "LayoutGroup";

            std::cout << i << ".\t" << &widget_list[i] << " (Layer: " << widget_list[i]->getLayer() << ", Selected: " << (selected_widget == widget_list[i]) << ", Hovered: " << (hovered_widget == widget_list[i]) << ") Type: " << type.toAnsiString() << std::endl;
        }
    }

    float Canvas::getDeltaTime()
    {
        return s_delta_time;
    }

    ResourceHandler<std::string, sf::Font> Fonts::s_fontTable;

    bool Fonts::load(const std::string& p_name, const std::string& p_filePath)
    {
        return s_fontTable.load(p_name, p_filePath);
    }
    sf::Font& Fonts::get(const std::string& p_name)
    {
        return s_fontTable.get(p_name);
    }

    Cursor::CursorData::CursorData()
    {
        for (int i = 0; i < 13; i++)
        {
            if(!system_cursors[i].loadFromSystem((sf::Cursor::Type)i))
            {
                std::cout << "Failed to load system cursor (" << i << ")!" << std::endl;
            }
        }
    }

    void Cursor::CursorData::setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        p_window.setMouseCursor(system_cursors[(int)p_type]);
    }

    Cursor::CursorData Cursor::s_data;

    void Cursor::setCursor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        s_data.setCursor(p_type, p_window);
    }

    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        return mouse_position;
    }

    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        mouse_position -= relative_to;
        return mouse_position;
    }

    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view) 
    {
        sf::Vector2f mousePosition = getMousePosition(window, view);
        sf::Vector2f distanceVector = mousePosition - sf::Vector2f(center.x + radius, center.y + radius);
        float distance = std::sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

        return distance < radius;
    }

    bool mouseOverRectangle(const sf::FloatRect& rectangle, sf::RenderWindow& window, sf::View* view) 
    {
        sf::Vector2f mousePosition = getMousePosition(window, view);
        return rectangle.contains(mousePosition);
    }

    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c)
    {
        std::wstringstream strstream;
        strstream << str.toWideString();
        std::vector<sf::String> segmentList;
        std::wstring segment;

        while (std::getline(strstream, segment, c))
        {
            segmentList.push_back(segment);
        }

        return segmentList;
    }

    float stringToFloat(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Split number into two parts.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        float sum = 0.f;
        float factor = std::pow(10.f, number_parts[0].getSize() - 1.f);
        float value = 0.f;
        if(number_parts.size() < 1) return 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < number_parts[0].getSize(); i++)
        {
            value = number_parts[0][i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        // Add numbers after the comma to sum.
        if(number_parts.size() >= 2)
        {
            factor = 0.1f;
            for (int i = 0; i < number_parts[1].getSize(); i++)
            {
                value = number_parts[1][i] - 0x30;
                sum += factor * value;
                factor /= 10.f;
            }
        }

        if(negative) return -sum;
        return sum;
    }

    int stringToInt(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Discard everthing after a comma.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        if(number_parts.size() < 1) return 0.f;
        sf::String splitted_string = number_parts[0];

        // Split number into two parts.
        float sum = 0.f;
        float factor = std::pow(10.f, splitted_string.getSize() - 1.f);
        float value = 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < splitted_string.getSize(); i++)
        {
            value = splitted_string[i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        
        if(negative) return -sum;
        return sum;
    }

    sf::String toUppercase(const sf::String& p_string)
    {
        sf::String uppercase_string;
        for (int i = 0; i < p_string.getSize(); i++)
        {
            // Add transformed char to new string.
            uppercase_string += toUppercase(p_string[i]);
        }
        return uppercase_string;
    }
    sf::Uint32 toUppercase(sf::Uint32 p_char)
    {
        // Find conversion to uppercase.
        if((p_char >= 0x61 && p_char <= 0x7a) || // ASCII letters.
            (p_char >= 0xe0 && p_char <= 0xfe && p_char != 0xf7)) // Extendend ANSI letters.
        {
            p_char -= 0x20;
        }

        return p_char;
    }

    unsigned int hexadecimalToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0x");
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        p_string = toUppercase(p_string);
        if(p_string.toAnsiString().find_first_not_of("0123456789ABCDEF") != std::string::npos) return 0;        

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(16, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            unsigned int value = 0;
            if(p_string[i] >= 0x30 && p_string[i] <= 0x39) value = p_string[i] - 0x30;
            else if(p_string[i] >= 0x41 && p_string[i] <= 0x46) value = p_string[i] - 0x37;
            sum += factor * value;
            factor /= 16;
        }
        return sum;
    }

    unsigned int binaryToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0b"); 
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        if(p_string.toAnsiString().find_first_not_of("01") != std::string::npos) return 0;

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(2, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            sum += p_string[i] == '1' ? factor : 0;
            factor /= 2;
        }
        
        return sum;
    }

    sf::String toBinaryString(unsigned int p_value)
    {
        sf::String string;

        while(p_value != 0)
        {
            char digit = '0' + (p_value & 1);
            string += digit;
            
            p_value >>= 1;
        }

        sf::String reversed_string;
        for (int i = string.getSize() - 1; i >= 0; i--)
        {
            reversed_string += string[i];
        }
        reversed_string.insert(0, "0b");
        
        return reversed_string;
    }

    sf::String toHexadecimalString(unsigned int p_value, bool p_uppercase)
    {
        sf::String string;
        int sum = 0;
        int exponent = 1;
        while(sum != p_value)
        {
            int digit = (p_value % (int)std::pow(16, exponent)) / std::pow(16, exponent - 1);
            if(digit <= 9)
            {
                string += (char)('0' + digit);
            }
            else
            {
                string += (char)(p_uppercase ? 'A' + (digit - 10) : 'a' + (digit - 10));
            }
            sum += digit * std::pow(16, exponent - 1);
            exponent++;
        }

        sf::String reversed_string;
        for (int i = string.getSize() - 1; i >= 0; i--)
        {
            reversed_string += string[i];
        }
        reversed_string.insert(0, "0x");

        return reversed_string;
    }
}