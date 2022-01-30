#include "JUI.hpp"
#include "Utils.hpp"
#include "Resources.hpp"

#include <iostream> /* std::cout for debugging */
#include <vector>

#ifdef WINDOWS
#include <windows.h>
#endif

namespace jui
{
    Widget::Widget(Canvas& p_canvas)
        : position(sf::Vector2f(100.f, 100.f)), 
        size(sf::Vector2f(100.f, 100.f)),
        window(p_canvas.window), 
        canvas(p_canvas)
    {
        setPosition(sf::Vector2f(100.f, 100.f));
        setSize(sf::Vector2f(100.f, 100.f));

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
        if(p_size.x < 0.f || p_size.y < 0.f) return;
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
        canvas.remove(this);
        for (int i = 0; i < canvas.getCount(); i++)
        {
            if(layer < canvas.get(i)->getLayer())
            {
                int insertTo = i == 0 ? 0 : i - 1;
                canvas.insert(this, insertTo);
                return;
            }
        }
        canvas.insert(this, canvas.getCount() - 1);
    }

    int Widget::getLayer()
    {
        return layer;
    }
    
    void Widget::orient(AnchorPoint p_point, const Widget& p_widget, const sf::Vector2f& p_offset) 
    {
        switch(p_point)
        {
            case AnchorPoint::TopLeft:
            setPosition(p_widget.getPosition() + p_offset);
            break;
            case AnchorPoint::Top:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x/2.f - getSize().x/2.f, p_widget.getPosition().y) + p_offset);
            break;
            case AnchorPoint::TopRight:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x - getSize().x, p_widget.getPosition().y) + p_offset);
            break;
            case AnchorPoint::Left:
            setPosition(sf::Vector2f(p_widget.getPosition().x, p_widget.getPosition().y + p_widget.getSize().y/2.f - getSize().y/2.f) + p_offset);
            break;
            case AnchorPoint::Middle:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x/2.f - getSize().x/2.f, p_widget.getPosition().y + p_widget.getSize().y/2.f - getSize().y/2.f) + p_offset);
            break;
            case AnchorPoint::Right:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x - getSize().x, p_widget.getPosition().y + p_widget.getSize().y/2.f - getSize().y/2.f) + p_offset);
            break;
            case AnchorPoint::BottomLeft:
            setPosition(sf::Vector2f(p_widget.getPosition().x, p_widget.getPosition().y + p_widget.getSize().y - getSize().y) + p_offset);
            break;
            case AnchorPoint::Bottom:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x/2.f - getSize().x/2.f, p_widget.getPosition().y + p_widget.getSize().y - getSize().y) + p_offset);
            break;
            case AnchorPoint::BottomRight:
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_widget.getSize().x - getSize().x, p_widget.getPosition().y + p_widget.getSize().y - getSize().y) + p_offset);
            break;
        }
    }
    
    void Widget::stretch(AnchorPoint p_point, const Widget& p_widget, float p_offset) 
    {
        switch(p_point)
        {
            case AnchorPoint::Top:
            setSize(sf::Vector2f(getSize().x, getSize().y + getPosition().y - (p_widget.getPosition().y + p_offset)));
            setPosition(sf::Vector2f(getPosition().x, p_widget.getPosition().y + p_offset));       
            break;
            case AnchorPoint::Left:
            setSize(sf::Vector2f(getSize().x + getPosition().x - (p_widget.getPosition().x + p_offset), getSize().y));
            setPosition(sf::Vector2f(p_widget.getPosition().x + p_offset, getPosition().y));
            break;
            case AnchorPoint::Right:
            setSize(sf::Vector2f((p_widget.getPosition().x + p_widget.getSize().x - p_offset) - getPosition().x, getSize().y));
            break;
            case AnchorPoint::Bottom:
            setSize(sf::Vector2f(getSize().x, (p_widget.getPosition().y + p_widget.getSize().y - p_offset) - getPosition().y));
            break;
        }
    }
    
    void Widget::refreshView() 
    {
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
    
    const sf::View& Widget::getView() const
    {
        return view;
    }

    Text::Text() :
    string             (),
    font               (NULL),
    character_size      (30),
    letter_spacing_factor(1.f),
    line_spacing_factor  (1.f),
    style              (Regular),
    fill_color          (255, 255, 255),
    outline_color       (0, 0, 0),
    outline_thickness   (0),
    vertices           (sf::Triangles),
    outline_vertices    (sf::Triangles),
    selection_vertices  (sf::Triangles),
    bounds             (),
    geometry_need_update (false)
    {

    }

    Text::Text(const sf::String& string, const sf::Font& font, unsigned int character_size) :
    string             (string),
    font               (&font),
    character_size      (character_size),
    letter_spacing_factor(1.f),
    line_spacing_factor  (1.f),
    style              (Regular),
    fill_color          (255, 255, 255),
    outline_color       (0, 0, 0),
    outline_thickness   (0),
    vertices           (sf::Triangles),
    outline_vertices    (sf::Triangles),
    selection_vertices  (sf::Triangles),
    bounds             (),
    geometry_need_update (true)
    {

    }

    void Text::setString(const sf::String& p_string)
    {
        if (string != p_string)
        {
            string = p_string;
            geometry_need_update = true;
        }
    }

    void Text::setFont(const sf::Font& p_font)
    {
        if (font != &p_font)
        {
            font = &p_font;
            geometry_need_update = true;
        }
    }

    void Text::setCharacterSize(unsigned int p_size)
    {
        if (character_size != p_size)
        {
            character_size = p_size;
            geometry_need_update = true;
        }
    }

    void Text::setLetterSpacing(float p_spacingFactor)
    {
        if (letter_spacing_factor != p_spacingFactor)
        {
            letter_spacing_factor = p_spacingFactor;
            geometry_need_update = true;
        }
    }

    void Text::setLineSpacing(float p_spacingFactor)
    {
        if (line_spacing_factor != p_spacingFactor)
        {
            line_spacing_factor = p_spacingFactor;
            geometry_need_update = true;
        }
    }

    void Text::setStyle(sf::Uint32 p_style)
    {
        if (style != p_style)
        {
            style = p_style;
            geometry_need_update = true;
        }
    }

    void Text::setFillColor(const sf::Color& p_color)
    {
        if (fill_color != p_color)
        {
            fill_color = p_color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!geometry_need_update)
            {
                for (std::size_t i = 0; i < vertices.getVertexCount(); ++i)
                    vertices[i].color = fill_color;
            }
        }
    }

    void Text::setOutlineColor(const sf::Color& p_color)
    {
        if (outline_color != p_color)
        {
            outline_color = p_color;

            // Change vertex colors directly, no need to update whole geometry
            // (if geometry is updated anyway, we can skip this step)
            if (!geometry_need_update)
            {
                for (std::size_t i = 0; i < outline_vertices.getVertexCount(); ++i)
                    outline_vertices[i].color = outline_color;
            }
        }
    }

    void Text::setOutlineThickness(float p_thickness)
    {
        if (outline_thickness != p_thickness)
        {
            outline_thickness = p_thickness;
            geometry_need_update = true;
        }
    }

    void Text::setSelection(int p_start_char, int p_end_char)
    {
        if(p_start_char < 0 || p_end_char >= string.getSize() || p_start_char > p_end_char) return;
        selection_vertices.clear();
        sf::Vector2f start_position = getInverseTransform().transformPoint(findCharacterPos(p_start_char));
        sf::Vector2f end_position = getInverseTransform().transformPoint(findCharacterPos(p_end_char));
        selection_vertices.append(sf::Vertex(sf::Vector2f(start_position.x, start_position.y), selection_color));
        selection_vertices.append(sf::Vertex(sf::Vector2f(end_position.x, start_position.y), selection_color));
        selection_vertices.append(sf::Vertex(sf::Vector2f(start_position.x, end_position.y + getCharacterSize()), selection_color));
        selection_vertices.append(sf::Vertex(sf::Vector2f(end_position.x, start_position.y), selection_color));
        selection_vertices.append(sf::Vertex(sf::Vector2f(end_position.x, end_position.y + getCharacterSize()), selection_color));
        selection_vertices.append(sf::Vertex(sf::Vector2f(start_position.x, end_position.y + getCharacterSize()), selection_color));
    }

    void Text::setSelectionColor(const sf::Color& p_color)
    {
        if(selection_color != p_color)
        {
            selection_color = p_color;
            for (int i = 0; i < selection_vertices.getVertexCount(); i++)
            {
                selection_vertices[i].color = selection_color;
            }
        }
    }
    
    const sf::Color& Text::getSelectionColor()
    {
        return selection_color;
    }

    const sf::String& Text::getString() const
    {
        return string;
    }

    const sf::Font* Text::getFont() const
    {
        return font;
    }

    unsigned int Text::getCharacterSize() const
    {
        return character_size;
    }

    float Text::getLetterSpacing() const
    {
        return letter_spacing_factor;
    }

    float Text::getLineSpacing() const
    {
        return line_spacing_factor;
    }

    sf::Uint32 Text::getStyle() const
    {
        return style;
    }

    const sf::Color& Text::getFillColor() const
    {
        return fill_color;
    }

    const sf::Color& Text::getOutlineColor() const
    {
        return outline_color;
    }

    float Text::getOutlineThickness() const
    {
        return outline_thickness;
    }

    sf::Vector2f Text::findCharacterPos(std::size_t p_index) const
    {
        // Make sure that we have a valid font
        if (!font)
            return sf::Vector2f();

        // Adjust the index if it's out of range
        if (p_index > string.getSize())
            p_index = string.getSize();

        // Precompute the variables needed by the algorithm
        bool  isBold          = style & Bold;
        float whitespaceWidth = font->getGlyph(L' ', character_size, isBold).advance;
        float letterSpacing   = ( whitespaceWidth / 3.f ) * ( letter_spacing_factor - 1.f );
        whitespaceWidth      += letterSpacing;
        float lineSpacing     = font->getLineSpacing(character_size) * line_spacing_factor;

        // Compute the position
        sf::Vector2f position;
        sf::Uint32 prevChar = 0;
        for (std::size_t i = 0; i < p_index; ++i)
        {
            sf::Uint32 curChar = string[i];

            // Apply the kerning offset
            position.x += font->getKerning(prevChar, curChar, character_size);
            prevChar = curChar;

            // Handle special characters
            switch (curChar)
            {
                case ' ':  position.x += whitespaceWidth;             continue;
                case '\t': position.x += whitespaceWidth * 4;         continue;
                case '\n': position.y += lineSpacing; position.x = 0; continue;
            }

            // For regular characters, add the advance offset of the glyph
            position.x += font->getGlyph(curChar, character_size, isBold).advance + letterSpacing;
        }

        // Transform the position to global coordinates
        position = getTransform().transformPoint(position);

        return position;
    }

    sf::FloatRect Text::getLocalBounds() const
    {
        ensureGeometryUpdate();

        return bounds;
    }

    sf::FloatRect Text::getGlobalBounds() const
    {
        return getTransform().transformRect(getLocalBounds());
    }

    void Text::draw(sf::RenderTarget& p_target, sf::RenderStates p_states) const
    {
        if (font)
        {
            ensureGeometryUpdate();

            p_states.transform *= getTransform();
            p_states.texture = &font->getTexture(character_size);

            // Only draw the outline if there is something to draw
            if (outline_thickness != 0)
                p_target.draw(outline_vertices, p_states);

            p_target.draw(vertices, p_states);

            // Draw selection.
            p_states.texture = nullptr;
            p_target.draw(selection_vertices, p_states);
        }
    }

    void Text::ensureGeometryUpdate() const
    {
        if (!font)
            return;

        // Do nothing, if geometry has not changed and the font texture has not changed
        if (!geometry_need_update /*&& font->getTexture(character_size).cacheId == fontTextureId*/)
            return;

        // Save the current fonts texture id
        //fontTextureId = font->getTexture(character_size).cacheId;

        // Mark geometry as updated
        geometry_need_update = false;

        // Clear the previous geometry
        vertices.clear();
        outline_vertices.clear();
        bounds = sf::FloatRect();

        // No text: nothing to draw
        if (string.isEmpty())
            return;

        // Compute values related to the text style
        bool  isBold             = style & Bold;
        bool  isUnderlined       = style & Underlined;
        bool  isStrikeThrough    = style & StrikeThrough;
        float italicShear        = (style & Italic) ? 0.209f : 0.f; // 12 degrees in radians
        float underlineOffset    = font->getUnderlinePosition(character_size);
        float underlineThickness = font->getUnderlineThickness(character_size);

        // Compute the location of the strike through dynamically
        // We use the center point of the lowercase 'x' glyph as the reference
        // We reuse the underline thickness as the thickness of the strike through as well
        sf::FloatRect xBounds = font->getGlyph(L'x', character_size, isBold).bounds;
        float strikeThroughOffset = xBounds.top + xBounds.height / 2.f;

        // Precompute the variables needed by the algorithm
        float whitespaceWidth = font->getGlyph(L' ', character_size, isBold).advance;
        float letterSpacing   = ( whitespaceWidth / 3.f ) * ( letter_spacing_factor - 1.f );
        whitespaceWidth      += letterSpacing;
        float lineSpacing     = font->getLineSpacing(character_size) * line_spacing_factor;
        float x               = 0.f;
        float y               = static_cast<float>(character_size);

        // Create one quad for each character
        float minX = static_cast<float>(character_size);
        float minY = static_cast<float>(character_size);
        float maxX = 0.f;
        float maxY = 0.f;
        sf::Uint32 prevChar = 0;
        for (std::size_t i = 0; i < string.getSize(); ++i)
        {
            sf::Uint32 curChar = string[i];

            // Skip the \r char to avoid weird graphical issues
            if (curChar == '\r')
                continue;

            // Apply the kerning offset
            x += font->getKerning(prevChar, curChar, character_size);

            // If we're using the underlined style and there's a new line, draw a line
            if (isUnderlined && (curChar == L'\n' && prevChar != L'\n'))
            {
                addLine(vertices, x, y, fill_color, underlineOffset, underlineThickness);

                if (outline_thickness != 0)
                    addLine(outline_vertices, x, y, outline_color, underlineOffset, underlineThickness, outline_thickness);
            }

            // If we're using the strike through style and there's a new line, draw a line across all characters
            if (isStrikeThrough && (curChar == L'\n' && prevChar != L'\n'))
            {
                addLine(vertices, x, y, fill_color, strikeThroughOffset, underlineThickness);

                if (outline_thickness != 0)
                    addLine(outline_vertices, x, y, outline_color, strikeThroughOffset, underlineThickness, outline_thickness);
            }

            prevChar = curChar;

            // Handle special characters
            if ((curChar == L' ') || (curChar == L'\n') || (curChar == L'\t'))
            {
                // Update the current bounds (min coordinates)
                minX = std::min(minX, x);
                minY = std::min(minY, y);

                switch (curChar)
                {
                    case L' ':  x += whitespaceWidth;     break;
                    case L'\t': x += whitespaceWidth * 4; break;
                    case L'\n': y += lineSpacing; x = 0;  break;
                }

                // Update the current bounds (max coordinates)
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);

                // Next glyph, no need to create a quad for whitespace
                continue;
            }

            // Apply the outline
            if (outline_thickness != 0)
            {
                const sf::Glyph& glyph = font->getGlyph(curChar, character_size, isBold, outline_thickness);

                float left   = glyph.bounds.left;
                float top    = glyph.bounds.top;
                float right  = glyph.bounds.left + glyph.bounds.width;
                float bottom = glyph.bounds.top  + glyph.bounds.height;

                // Add the outline glyph to the vertices
                addGlyphQuad(outline_vertices, sf::Vector2f(x, y), outline_color, glyph, italicShear, outline_thickness);

                // Update the current bounds with the outlined glyph bounds
                minX = std::min(minX, x + left   - italicShear * bottom - outline_thickness);
                maxX = std::max(maxX, x + right  - italicShear * top    - outline_thickness);
                minY = std::min(minY, y + top    - outline_thickness);
                maxY = std::max(maxY, y + bottom - outline_thickness);
            }

            // Extract the current glyph's description
            const sf::Glyph& glyph = font->getGlyph(curChar, character_size, isBold);

            // Add the glyph to the vertices
            addGlyphQuad(vertices, sf::Vector2f(x, y), fill_color, glyph, italicShear);

            // Update the current bounds with the non outlined glyph bounds
            if (outline_thickness == 0)
            {
                float left   = glyph.bounds.left;
                float top    = glyph.bounds.top;
                float right  = glyph.bounds.left + glyph.bounds.width;
                float bottom = glyph.bounds.top  + glyph.bounds.height;

                minX = std::min(minX, x + left  - italicShear * bottom);
                maxX = std::max(maxX, x + right - italicShear * top);
                minY = std::min(minY, y + top);
                maxY = std::max(maxY, y + bottom);
            }

            // Advance to the next character
            x += glyph.advance + letterSpacing;
        }

        // If we're using the underlined style, add the last line
        if (isUnderlined && (x > 0))
        {
            addLine(vertices, x, y, fill_color, underlineOffset, underlineThickness);

            if (outline_thickness != 0)
                addLine(outline_vertices, x, y, outline_color, underlineOffset, underlineThickness, outline_thickness);
        }

        // If we're using the strike through style, add the last line across all characters
        if (isStrikeThrough && (x > 0))
        {
            addLine(vertices, x, y, fill_color, strikeThroughOffset, underlineThickness);

            if (outline_thickness != 0)
                addLine(outline_vertices, x, y, outline_color, strikeThroughOffset, underlineThickness, outline_thickness);
        }

        // Update the bounding rectangle
        bounds.left = minX;
        bounds.top = minY;
        bounds.width = maxX - minX;
        bounds.height = maxY - minY;
    }

    // Add an underline or strikethrough line to the vertex array
    void Text::addLine(sf::VertexArray& p_vertices, float p_line_length, float p_line_top, const sf::Color& p_color, float p_offset, float p_thickness, float p_outline_thickness) const
    {
        float top = std::floor(p_line_top + p_offset - (p_thickness / 2) + 0.5f);
        float bottom = top + std::floor(p_thickness + 0.5f);

        p_vertices.append(sf::Vertex(sf::Vector2f(-p_outline_thickness,                 top    - p_outline_thickness), p_color, sf::Vector2f(1, 1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_line_length + p_outline_thickness,  top    - p_outline_thickness), p_color, sf::Vector2f(1, 1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(-p_outline_thickness,                 bottom + p_outline_thickness), p_color, sf::Vector2f(1, 1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(-p_outline_thickness,                 bottom + p_outline_thickness), p_color, sf::Vector2f(1, 1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_line_length + p_outline_thickness,  top    - p_outline_thickness), p_color, sf::Vector2f(1, 1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_line_length + p_outline_thickness,  bottom + p_outline_thickness), p_color, sf::Vector2f(1, 1)));
    }

    // Add a glyph quad to the vertex array
    void Text::addGlyphQuad(sf::VertexArray& p_vertices, sf::Vector2f p_position, const sf::Color& p_color, const sf::Glyph& p_glyph, float p_italicShear, float p_outline_thickness) const
    {
        float padding = 1.0;

        float left   = p_glyph.bounds.left - padding;
        float top    = p_glyph.bounds.top - padding;
        float right  = p_glyph.bounds.left + p_glyph.bounds.width + padding;
        float bottom = p_glyph.bounds.top  + p_glyph.bounds.height + padding;

        float u1 = static_cast<float>(p_glyph.textureRect.left) - padding;
        float v1 = static_cast<float>(p_glyph.textureRect.top) - padding;
        float u2 = static_cast<float>(p_glyph.textureRect.left + p_glyph.textureRect.width) + padding;
        float v2 = static_cast<float>(p_glyph.textureRect.top  + p_glyph.textureRect.height) + padding;

        // Every quad is made up from two triangles, therefore 6 vertices are added to the passed vertex array.
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left  - p_italicShear * top    - p_outline_thickness, p_position.y + top    - p_outline_thickness), p_color, sf::Vector2f(u1, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right - p_italicShear * top    - p_outline_thickness, p_position.y + top    - p_outline_thickness), p_color, sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left  - p_italicShear * bottom - p_outline_thickness, p_position.y + bottom - p_outline_thickness), p_color, sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + left  - p_italicShear * bottom - p_outline_thickness, p_position.y + bottom - p_outline_thickness), p_color, sf::Vector2f(u1, v2)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right - p_italicShear * top    - p_outline_thickness, p_position.y + top    - p_outline_thickness), p_color, sf::Vector2f(u2, v1)));
        p_vertices.append(sf::Vertex(sf::Vector2f(p_position.x + right - p_italicShear * bottom - p_outline_thickness, p_position.y + bottom - p_outline_thickness), p_color, sf::Vector2f(u2, v2)));
    }

    TextField::TextField(Canvas& p_canvas, sf::Font& p_font) 
        : Widget(p_canvas), font(p_font)//, icon(nullptr)
    {
        text.setFont(font);
        text.setFillColor(sf::Color::Black);
        text.setString(L"New Text");
        setPosition(getPosition());
        orientText(jui::Middle, sf::Vector2f());
    }
    
    void TextField::setPosition(const sf::Vector2f& p_position) 
    {
        text.setPosition(p_position + (text.getPosition() - getPosition()));
        Widget::setPosition (p_position);
    }

    void TextField::orientText(AnchorPoint p_anchor, const sf::Vector2f& p_offset/*, bool p_allow_overflow*/)
    {
        // We need to subtract text.getLocalBounds().top/left from the desired pixel position to get the true origin of the text object.
        sf::Vector2f localOffset = sf::Vector2f(text.getLocalBounds().left, text.getLocalBounds().top);
        switch(p_anchor)
        {
            case AnchorPoint::TopLeft:
            text.setPosition(getPosition() + p_offset - localOffset);
            break;
            case AnchorPoint::Top:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x/2.f - text.getGlobalBounds().width/2.f, getPosition().y) + p_offset - localOffset);
            break;
            case AnchorPoint::TopRight:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x - text.getGlobalBounds().width, getPosition().y) + p_offset - localOffset);
            break;
            case AnchorPoint::Left:
            text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y/2.f - text.getGlobalBounds().height/2.f) + p_offset - localOffset);
            break;
            case AnchorPoint::Middle:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x/2.f - text.getGlobalBounds().width/2.f, getPosition().y + getSize().y/2.f - text.getGlobalBounds().height/2.f) + p_offset - localOffset);
            break;
            case AnchorPoint::Right:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x - text.getGlobalBounds().width, getPosition().y + getSize().y/2.f - text.getGlobalBounds().height/2.f) + p_offset - localOffset);
            break;
            case AnchorPoint::BottomLeft:
            text.setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y - text.getGlobalBounds().height) + p_offset - localOffset);
            break;
            case AnchorPoint::Bottom:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x/2.f - text.getGlobalBounds().width/2.f, getPosition().y + getSize().y - text.getGlobalBounds().height) + p_offset - localOffset);
            break;
            case AnchorPoint::BottomRight:
            text.setPosition(sf::Vector2f(getPosition().x + getSize().x - text.getGlobalBounds().width, getPosition().y + getSize().y - text.getGlobalBounds().height) + p_offset - localOffset);
            break;
        }
        text.setPosition((int)text.getPosition().x, (int)text.getPosition().y); // Make sure the text doesn't get blurry due to floating point positions.
    }

    // void TextField::setIcon(const sf::Texture* p_texture, AnchorPoint p_position = AnchorPoint::Left, const sf::Vector2f& p_size = sf::Vector2f(32.f, 32.f))
    // {
    //     icon.setTexture(p_texture);
    //     icon.setSize(p_size);
    // }

    void TextField::draw() const 
    {
        Widget::draw();

        window.setView(view);
        window.draw(text);
    }

    Button::Button(Canvas& p_canvas, sf::Font& p_font)
        : Widget::Widget(p_canvas), text_field(p_canvas, p_font)
    {
        setSize(sf::Vector2f(300.f, 40.f));
        text_field.setSize(sf::Vector2f(300.f, 40.f));
        text_field.setFillColor(sf::Color::Transparent);
        text_field.text.setString("New Button");
        text_field.text.setCharacterSize(15);
        text_field.orientText(jui::Middle);
        text_field.setSelectable(false);
        text_field.is_draggable = false;
        text_field.is_resizable = false;
        text_field.setMask(this);

        f_event = [](){};
    }

    void Button::setPosition(const sf::Vector2f& p_position)
    {
        text_field.setPosition(p_position);
        Widget::setPosition(p_position);
    }

    void Button::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        text_field.setSize(p_size);
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

    void Button::setString(const sf::String& p_string)
    {
        text_field.text.setString(p_string);
    }
    const sf::String& Button::getString()
    {
        return text_field.text.getString();
    }

    void Button::orientText(AnchorPoint p_orientation, const sf::Vector2f& p_offset)
    {
        text_field.orientText(p_orientation, p_offset);
    }

    void Button::setTextColor(const sf::Color& p_color)
    {
        text_field.text.setFillColor(p_color);
    }

    void Button::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        text_field.setLayer(p_layer);
    }
    void Button::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        text_field.setVisible(p_state);
    }
    void Button::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        text_field.setUpdated(p_state);
    }
    void Button::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        text_field.setSelectable(p_state);
    }

    ScrollBar::ScrollBar(Canvas& p_canvas, sf::Font& p_font, Orientation p_orientation) 
        : Widget::Widget(p_canvas), move_up_button(p_canvas, p_font), move_down_button(p_canvas, p_font), orientation(p_orientation)
    {
        is_draggable = false;
        is_resizable = false;
        move_up_button.is_draggable = false;
        move_down_button.is_draggable = false;
        move_up_button.is_resizable = false;
        move_down_button.is_resizable = false;

        move_up_button.standard_color = sf::Color(100, 100, 100);
        move_up_button.highlight_color = sf::Color(200, 200, 200);
        move_down_button.standard_color = sf::Color(100, 100, 100);
        move_down_button.highlight_color = sf::Color(200, 200, 200);
        setFillColor(sf::Color(100, 100, 100));
        scroll_handle.setFillColor(sf::Color(200, 200, 200));

        move_up_button.setMask(this);
        move_down_button.setMask(this);

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
            scroll_handle.setSize(sf::Vector2f(getSize().x, size_ratio * scroll_bar_size));
            scroll_handle.setPosition(sf::Vector2f(getPosition().x, getPosition().y + move_down_button.getSize().y + position_ratio * scroll_bar_size));
        }
        else if (orientation == Horizontal)
        {
            float scroll_bar_size = (getSize().x - move_down_button.getSize().x * 2);
            scroll_handle.setSize(sf::Vector2f(size_ratio * scroll_bar_size, getSize().y));
            scroll_handle.setPosition(sf::Vector2f(getPosition().x + move_down_button.getSize().x + position_ratio * scroll_bar_size, getPosition().y));
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
            move_up_button.setString(L"\u25b2");
            move_down_button.setString(L"\u25bc");
        }
        else if (orientation == Horizontal)
        {
            move_up_button.setString(L"\u25c4");
            move_down_button.setString(L"\u25ba");
        }
        move_up_button.orientText(jui::Middle);
        move_down_button.orientText(jui::Middle);
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

    void ScrollBar::draw() const
    {
        Widget::draw();
        window.draw(scroll_handle);
    }

    void ScrollBar::updateEvents(sf::Event& p_event) 
    {
        // Deselect the handle when the left mouse is released.
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left && scroll_handle.getGlobalBounds().contains(getMousePosition(window, &view)))
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
        if(canvas.getSelected() == this)
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
                    setScrollState(state.scroll_handle_position + (difference.x / getSize().y) * state.scroll_list_length, state.scroll_handle_size, state.scroll_list_length);
                }
                last_mouse_position = getMousePosition(window, &view);
            }
        }

        if(scroll_handle.getGlobalBounds().contains(getMousePosition(window, &view)))
        {
            scroll_handle.setFillColor(sf::Color(150, 150, 150));
        }
        else
        {
            scroll_handle.setFillColor(sf::Color(200, 200, 200));
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
    }
    void ScrollBar::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        move_up_button.setVisible(p_state);
        move_down_button.setVisible(p_state);
    }
    void ScrollBar::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        move_up_button.setUpdated(p_state);
        move_down_button.setUpdated(p_state);
    }
    void ScrollBar::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        move_up_button.setSelectable(p_state);
        move_down_button.setSelectable(p_state);
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
        scroll_bar.setPosition(sf::Vector2f(getPosition().x + getSize().x - scroll_bar.getSize().x, getPosition().y));
    }

    void ScrollList::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        scroll_bar.setPosition(sf::Vector2f(getPosition().x + getSize().x - scroll_bar.getSize().x, getPosition().y));
        scroll_bar.setSize(sf::Vector2f(scroll_bar.getSize().x, getSize().y));
    }

    void ScrollList::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        scroll_bar.setLayer(p_layer);
        content.setLayer(p_layer);
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
        dropdown_button.is_draggable = false;
        scrolllist.is_draggable = false;
        scrolllist.setVisible(false);
        setPosition(sf::Vector2f(100.f, 100.f));
        setSize(sf::Vector2f(300.f, 40.f));
        scrolllist.setFillColor(sf::Color(200, 200, 200, 200));

        dropdown_button.setString("New DropDownList");
        dropdown_button.orientText(jui::Left, sf::Vector2f(10.f, 0.f));
        dropdown_button.setOnClickEvent([&]()
        {
            scrolllist.setVisible(true);
        });
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
    }

    void DropDownList::updateEvents(sf::Event& p_event)
    {
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                if(!scrolllist.mouseOver() && !dropdown_button.mouseOver())
                {
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

    void DropDownList::setButtonColor(const sf::Color& p_color)
    {
        dropdown_button.standard_color = p_color;
    }    
    void DropDownList::setButtonTextColor(const sf::Color& p_color)
    {
        dropdown_button.setTextColor(p_color);
    }

    void DropDownList::setString(const sf::String& p_string)
    {
        dropdown_button.setString(p_string);
        dropdown_button.orientText(jui::Left, sf::Vector2f(10.f, 0.f));
    }

    sf::String DropDownList::getString()
    {
        return dropdown_button.getString();
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
            button->setString(p_elements[i]);
            button->orientText(jui::Middle);
            button->standard_color = sf::Color(240, 240, 240);
            button->is_draggable = false;
            button->is_resizable = false;

            // The button click listener gets a copy of a pointer to this DropDownList, as well as a pointer to itself.
            button->setOnClickEvent([this, button]()
            {
                scrolllist.setVisible(false);
                dropdown_button.setString(button->getString());
                dropdown_button.orientText(jui::Left, sf::Vector2f(10.f, 0.f));
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
        : Widget::Widget(p_canvas), text("", p_font), placeholder_text("Input here...", p_font), cursor()
    {
        is_draggable = false;
        is_resizable = false;

        setPosition(sf::Vector2f(100.f, 100.f));
        setSize(sf::Vector2f(300.f, 40.f));
        setCharacterSize(15);

        text.setFillColor(sf::Color::Black);
        placeholder_text.setFillColor(placeholder_text_color);
        cursor.setFillColor(sf::Color::Black);

        selection.setPosition(100.f, 100.f);
        selection.setFillColor(text_selection_color);
        selection.setSize(sf::Vector2f(20.f, selection.getSize().y));
        
        refreshCursor();

        f_customIsCharValid = [](sf::Uint32 p_char){ return true; };
        f_onSelect = [](){};
        f_onDeselect = [](){};

    }

    void InputField::setPosition(const sf::Vector2f& p_position)
    {
        text.setPosition(sf::Vector2f(p_position.x + (text.getPosition().x - getPosition().x), p_position.y + getSize().y/2.f - text.getCharacterSize()/2.f - text.getLocalBounds().top));
        placeholder_text.setPosition(text.getPosition());
        selection.setPosition(p_position + (selection.getPosition() - getPosition()));
        Widget::setPosition(p_position);
        refreshCursor();
    }

    void InputField::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        setPosition(getPosition());
    }

    void InputField::setCharacterSize(unsigned int p_size)
    {
        text.setCharacterSize(p_size);
        placeholder_text.setCharacterSize(p_size);
        cursor.setSize(sf::Vector2f(2.f, p_size));
        selection.setSize(sf::Vector2f(selection.getSize().x, p_size));
        setPosition(getPosition());
    }

    void InputField::setPlaceholderText(const sf::String& p_string)
    {
        placeholder_text.setString(p_string);
    }

    void InputField::draw() const
    {
        Widget::draw();
        window.setView(view);
        window.draw(text);
        if(canvas.getSelected() == this)
        {
            window.draw(cursor);
            window.draw(selection);
        }
        else
        {
            if(text.getString().getSize() == 0)
            {
                window.draw(placeholder_text);
            }
        }
    }

    void InputField::refreshCursor()
    {
        if(cursor_index < 0)
        {
            cursor_index = 0;
        }
        if(cursor_index > text.getString().getSize())
        {
            cursor_index = text.getString().getSize();
        }

        sf::Vector2f next_cursor_index = text.findCharacterPos(cursor_index);

        // Update text position.
        if(next_cursor_index.x > getPosition().x + getSize().x)
        {
            text.move(sf::Vector2f(getPosition().x + getSize().x - next_cursor_index.x - 10.f, 0.f));
        }
        if(next_cursor_index.x < getPosition().x)
        {
            text.move(sf::Vector2f(getPosition().x - next_cursor_index.x + 10.f, 0.f));
        }

        // Update cursor position.
        cursor.setPosition(text.findCharacterPos(cursor_index));
    }

    void InputField::refreshSelection()
    {
        sf::Vector2f drag_cursor_position = text.findCharacterPos(drag_cursor_index);
        sf::Vector2f cursor_position = text.findCharacterPos(cursor_index);
        selection.setPosition(cursor_position);
        selection.setSize(sf::Vector2f(drag_cursor_position.x - cursor_position.x, selection.getSize().y));
    }

    void InputField::setInputType(sf::Uint32 p_type)
    {
        type = p_type;
    }

    void InputField::updateLogic()
    {
        if(canvas.getSelected() != this)
        {
            if(canvas.getHovered() == this)
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

        if(is_making_mouse_selection)
        {
            drag_cursor_index = getCharIndexByPosition(text, getMousePosition(window, &view));
            sf::Vector2f drag_cursor_position = text.findCharacterPos(drag_cursor_index);
            if(drag_cursor_position.x > getPosition().x + getSize().x)
            {
                text.move(sf::Vector2f(getPosition().x + getSize().x - drag_cursor_position.x - 10.f, 0.f));
            }
            else if (drag_cursor_position.x < getPosition().x)
            {
                text.move(sf::Vector2f(getPosition().x - drag_cursor_position.x + 10.f, 0.f));
            }

            cursor.setPosition(text.findCharacterPos(cursor_index));
            refreshSelection();
        }
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

    void InputField::updateEvents(sf::Event& p_event)
    {
        if(canvas.getSelected() == this)
        {
            if(p_event.type == sf::Event::KeyPressed)
            {
                if(p_event.key.code == sf::Keyboard::Left)
                {
                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index--;
                        refreshCursor();
                        refreshSelection();
                    }
                    else if(cursor_index != drag_cursor_index)
                    {
                        is_making_mouse_selection = false;
                        selection.setSize(sf::Vector2f(0.f, selection.getSize().y));
                        cursor_index = std::min(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                    else
                    {
                        cursor_index--;
                        refreshCursor();
                        drag_cursor_index = cursor_index;
                    }
                }
                if(p_event.key.code == sf::Keyboard::Right)
                {
                    if(is_making_shift_selection)
                    {
                        is_making_mouse_selection = false;
                        cursor_index++;
                        refreshCursor();
                        refreshSelection();
                    }
                    else if(cursor_index != drag_cursor_index)
                    {
                        is_making_mouse_selection = false;
                        selection.setSize(sf::Vector2f(0.f, selection.getSize().y));
                        cursor_index = std::max(cursor_index, drag_cursor_index);
                        refreshCursor();
                        drag_cursor_index = cursor_index;
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
            if(p_event.type == sf::Event::MouseButtonPressed)
            {
                if(p_event.mouseButton.button == sf::Mouse::Left)
                {
                    cursor_index = getCharIndexByPosition(text, getMousePosition(window, &view));
                    drag_cursor_index = cursor_index;
                    refreshCursor();
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
            if(p_event.type == sf::Event::TextEntered)
            {
                sf::Uint32 input = p_event.text.unicode;

                // Crtl + C (3)
                if(input == 3)
                {
                    #ifdef WINDOWS
                    // If the user made a selection.
                    if(drag_cursor_index != cursor_index)
                    {
                        sf::String toCopy = text.getString().substring(std::min(cursor_index, drag_cursor_index), std::abs(cursor_index - drag_cursor_index));
                        copyToClipboard(toCopy);
                    }
                    #endif
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
                    selection.setSize(sf::Vector2f(0.f, selection.getSize().y));
                    text.setString(modified_string);
                    refreshCursor();
                }

                // Crtl + V (22)
                if(input == 22)
                {
                    #ifdef WINDOWS
                    sf::String toInsert = getClipboardText();
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
                    #endif
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
        if(p_event.type == sf::Event::MouseMoved)
        {
            bool is_currently_hovered = canvas.getHovered() == this;

            if(is_currently_hovered && !was_hovered)
            {
                Cursor::setCusor(sf::Cursor::Text, window);
            }
            if(!is_currently_hovered && was_hovered)
            {
                Cursor::setCusor(sf::Cursor::Arrow, window);
            }

            was_hovered = is_currently_hovered;
        }
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            bool is_currently_selected = canvas.getSelected() == this;
            if(!was_selected && is_currently_selected)
            {
                refreshLimitedFormat();
                f_onSelect();
            }
            if(was_selected && !is_currently_selected)
            {
                refreshLimitedFormat();
                f_onDeselect();
            }
            was_selected = is_currently_selected;
        }
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
            if(text.getString().getSize() == 10 && (text.getString()[2] != '.' || text.getString()[5] != '.'))
            {
                date_error = true;
            }
            else
            {
                date_error = false;
            }
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

    #ifdef WINDOWS
    sf::String InputField::getClipboardText()
    {
        // Try opening the clipboard
        if (! OpenClipboard(nullptr))
            return "";	

        // Get handle of clipboard object for ANSI text
        HANDLE hData = GetClipboardData(CF_UNICODETEXT); // 13 == CF_UNICODETEXT (https://docs.microsoft.com/en-us/windows/win32/dataxchg/standard-clipboard-formats)
        if (hData == nullptr)
            return "";	

        // Lock the handle to get the actual text pointer
        wchar_t* pszText = static_cast<wchar_t*>( GlobalLock(hData) );
        if (pszText == nullptr)
            return "";	

        // Save text in a string class instance
        sf::String text( pszText );

        // Release the lock
        GlobalUnlock( hData );

        // Release the clipboard.
        CloseClipboard();

        return text;
    }

    void InputField::copyToClipboard(const sf::String& p_string)
    {
        // No need to copy an empty string to clipboard.
        if(p_string.getSize() == 0) return;

        // Try opening the clipboard
        if (! OpenClipboard(nullptr))
            return;	
        
        const wchar_t* str_data = p_string.toWideString().c_str();
        EmptyClipboard();	
        HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(WCHAR) * (wcslen(str_data) + 1));
        if (!hClipboardData){
            CloseClipboard();
            return;
        }
        WCHAR* pchData;
        pchData = (WCHAR*)GlobalLock(hClipboardData);
        wcscpy(pchData, str_data);
        GlobalUnlock(hClipboardData);
        SetClipboardData(CF_UNICODETEXT, hClipboardData);

        // Release the clipboard.
        CloseClipboard();
    }
    #endif

    Tabs::Tabs(Canvas& p_canvas, sf::Font& p_font) 
        : Widget::Widget(p_canvas), button_background(), tabbar_height(40.f), horizontal_spacing(10.f), font(p_font)
    {
        setPosition(sf::Vector2f(100.f, 100.f));
        setSize(sf::Vector2f(300.f, 300.f));
        button_background.setFillColor(sf::Color(200, 200, 200));
    }

    Tabs::~Tabs()
    {
        for (int i = 0; i < tab_list.size(); i++)
        {
            remove(i);
        }
    }

    void Tabs::setPosition(const sf::Vector2f& p_position)
    {
        Widget::setPosition(p_position);
        button_background.setPosition(p_position);
        refreshUI();
    }

    void Tabs::setSize(const sf::Vector2f& p_size)
    {
        Widget::setSize(p_size);
        button_background.setSize(sf::Vector2f(p_size.x, tabbar_height));
        refreshUI();
    }

    void Tabs::setBarColor(const sf::Color& p_color)
    {
        button_background.setFillColor(p_color);
    }

    void Tabs::setBarHeight(float p_height)
    {
        tabbar_height = p_height;
        refreshUI();
    }

    void Tabs::setBarMargin(float p_left, float p_right, float p_top, float p_bottom)
    {
        left_bar_margin = p_left;
        right_bar_margin = p_right;
        top_bar_margin = p_top;
        bottom_bar_margin = p_bottom;
    }

    void Tabs::showTab(int p_index)
    {
        for (int i = 0; i < tab_list.size(); i++)
        {
            if(i != p_index)
            {
                tab_list[i].button->standard_color = standard_color;
                tab_list[i].widget->setVisible(false);
            }
            else
            {
                tab_list[i].button->standard_color = selected_color;
                tab_list[i].widget->setVisible(true);
            }
        }
    }

    void Tabs::setLayer(int p_layer)
    {
        Widget::setLayer(p_layer);
        for (int i = 0; i < tab_list.size(); i++)
        {
            tab_list[i].button->setLayer(p_layer);
        }
    }
    void Tabs::setVisible(bool p_state)
    {
        Widget::setVisible(p_state);
        for (int i = 0; i < tab_list.size(); i++)
        {
            tab_list[i].button->setVisible(p_state);
        }
    }
    void Tabs::setUpdated(bool p_state)
    {
        Widget::setUpdated(p_state);
        for (int i = 0; i < tab_list.size(); i++)
        {
            tab_list[i].button->setUpdated(p_state);
        }
    }
    void Tabs::setSelectable(bool p_state)
    {
        Widget::setSelectable(p_state);
        for (int i = 0; i < tab_list.size(); i++)
        {
            tab_list[i].button->setSelectable(p_state);
        }
    }

    void Tabs::draw() const
    {
        Widget::draw();
        window.setView(view);
        window.draw(button_background);
    }

    void Tabs::add(Widget* p_widget, const sf::String& p_name)
    {
        for (int i = 0; i < tab_list.size(); i++)
        {
            if(p_widget == tab_list[i].widget)
            {
                return;
            }
        }
        
        Button* button = new Button(canvas, font);
        button->setString(p_name);
        button->is_draggable = false;
        button->is_resizable = false;
        button->highlight_color = sf::Color(50, 50, 50);
        button->setMask(this);
        button->setOnClickEvent([this, button, p_widget]()
        {
            for (int i = 0; i < tab_list.size(); i++)
            {
                tab_list[i].button->standard_color = standard_color;
                tab_list[i].widget->setVisible(false);
            }
            
            button->standard_color = selected_color;
            p_widget->setVisible(true);
        });
        
        tab_list.push_back({ p_widget, button, p_name });
        refreshUI();
    }

    void Tabs::remove(int p_index)
    {
        delete tab_list[p_index].button;
        tab_list.erase(tab_list.begin() + p_index);
    }

    void Tabs::refreshUI()
    {
        float button_x_size = (getSize().x - horizontal_spacing * (tab_list.size() - 1) - right_bar_margin - left_bar_margin) / tab_list.size();
        for (int i = 0; i < tab_list.size(); i++)
        {
            tab_list[i].button->setSize(sf::Vector2f(button_x_size, tabbar_height - top_bar_margin - bottom_bar_margin));
            tab_list[i].button->setPosition(sf::Vector2f(left_bar_margin + getPosition().x + i * button_x_size + i * horizontal_spacing, top_bar_margin + getPosition().y));
            tab_list[i].button->orientText(jui::Middle);
        }
    }

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
        
        // Dragging and resizing.
        if(!is_resizing)
        {
            drag();
        }
        resize();

        if(!is_resizing && !is_dragging)
        {
            last_mouse_position = getMousePosition(window, &view);
        }
    }
    
    void Canvas::updateAllEvents(sf::Event& p_event) 
    {
        if(p_event.type == sf::Event::MouseButtonPressed)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                // Dragging.
                is_dragging = true;
                // Resizing.
                Widget* current_widget = getSelected();
                if(current_widget != nullptr)
                {
                    resizing_edge = mouseOverRectEdge(sf::FloatRect(current_widget->getPosition(), current_widget->getSize()), 10.f, window, &view);
                    if(resizing_edge > 0)
                    {
                        resize_origin = sf::Vector2f(current_widget->getPosition() + current_widget->getSize());
                        is_resizing = true;
                    }
                }
                
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
        if(p_event.type == sf::Event::MouseButtonReleased)
        {
            if(p_event.mouseButton.button == sf::Mouse::Left)
            {
                // Dragging.
                is_dragging = false;
                // Resizing.
                is_resizing = false;
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
    }

    void Canvas::insert(Widget* p_widget, int p_index) 
    {
        // Check if widget alreay exists.
        if(find(p_widget) != -1) return;

        // Add widget.
        widget_list.insert(widget_list.begin() + p_index, p_widget);
    }

    int Canvas::find(Widget* p_widget)
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

    Widget* Canvas::get(int p_index)
    {
        return widget_list[p_index];
    }
    
    int Canvas::getCount()
    {
        return widget_list.size();
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
        // for (int i = 0; i < widget_list.size(); i++)
        // {
        //     widget_list[i]->setOutlineThickness(0.f);
        // }
        selected_widget = p_widget;
        //if(selected_widget == nullptr) return;
        //selected_widget->setOutlineColor(selected_color);
        //selected_widget->setOutlineThickness(1.f);
    }

    void Canvas::drag() 
    {
        // Drag if mouse was pressed and a widget is selected.
        if(is_dragging && selected_widget != nullptr && selected_widget->is_draggable)
        {
            sf::Vector2f current_mouse_position = getMousePosition(window, &view);
            sf::Vector2f difference = current_mouse_position - last_mouse_position;
            last_mouse_position = current_mouse_position;

            selected_widget->move(difference);
        }
    }
    
    void Canvas::resize() 
    {
        Widget* current_widget = getSelected();
        if(is_resizing && current_widget != nullptr && current_widget->is_resizable)
        {
            // Calculate difference between current and last mouse position.
            sf::Vector2f current_mouse_position = getMousePosition(window, &view);
            sf::Vector2f difference = current_mouse_position - last_mouse_position;
            last_mouse_position = current_mouse_position;

            // Resize depending on the selected edge.
            if(resizing_edge == 2)
            {
                current_widget->setSize(current_widget->getSize() + sf::Vector2f(difference.x, 0.f));
            }
            if(resizing_edge == 3)
            {
                current_widget->setSize(current_widget->getSize() + sf::Vector2f(0.f, difference.y));
            }
            if(resizing_edge == 1)
            {
                current_widget->setPosition(current_widget->getPosition() + sf::Vector2f(0.f, difference.y));
                current_widget->setSize(sf::Vector2f(current_widget->getSize().x, resize_origin.y - current_widget->getPosition().y));

                // Clamping.
                if(current_widget->getPosition().y > resize_origin.y - 1.f)
                {
                    current_widget->setPosition(sf::Vector2f(current_widget->getPosition().x, resize_origin.y - 1.f));
                }
            }
            if(resizing_edge == 4)
            {
                current_widget->setPosition(current_widget->getPosition() + sf::Vector2f(difference.x, 0.f));
                current_widget->setSize(sf::Vector2f(resize_origin.x - current_widget->getPosition().x, current_widget->getSize().y));

                // Clamping.
                if(current_widget->getPosition().x > resize_origin.x - 1.f)
                {
                    current_widget->setPosition(sf::Vector2f(resize_origin.x - 1.f, current_widget->getPosition().y));
                }
            }

            // Clamping resizing in x and y direction.
            if(current_widget->getSize().x < 1.f)
            {
                current_widget->setSize(sf::Vector2f(1.f, current_widget->getSize().y));
            }
            if(current_widget->getSize().y < 1.f)
            {
                current_widget->setSize(sf::Vector2f(current_widget->getSize().x, 1.f));
            }
        }
    }

    void Canvas::print()
    {
        std::cout << "Canvas (" << this << ")" << std::endl;
        for (int i = 0; i < widget_list.size(); i++)
        {
            std::cout << i << ". " << &widget_list[i] << "(Layer: " << widget_list[i]->getLayer() << ", Selected: " << (selected_widget == widget_list[i]) << ")" << std::endl;
        }
    }

    void Canvas::setDeltaTime(float& p_delta_time)
    {
        delta_time = &p_delta_time;
    }

    float Canvas::getDeltaTime()
    {
        if(delta_time == nullptr) return 0.f;
        return *delta_time;
    }
}