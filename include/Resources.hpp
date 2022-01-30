#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

namespace jui
{
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

            void setCusor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
        };
        static CursorData s_data;
    public:
        static void setCusor(sf::Cursor::Type p_type, sf::RenderWindow& p_window);
    };

    class Icon : public sf::Drawable
    {
    private:
        const sf::Texture* texture;
        sf::Sprite shape;
    public:
        Icon(const sf::Texture* p_texture);

        void setTexture(const sf::Texture* p_texture);

        void setPosition(const sf::Vector2f& p_position);
        const sf::Vector2f& getPosition();
        void setSize(const sf::Vector2f& p_size);
        sf::Vector2f getSize();
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
};
