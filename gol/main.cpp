#include <chrono>
#include <iostream>
#include <random>

#include <SFML\Graphics.hpp>
#include <SFML\System\Time.hpp>

typedef unsigned char byte;
typedef unsigned int uint;

const uint w = 256;
const uint h = 100;
const uint c = 3;

const uint W = w*c, H = h*c;

class Map
{
private:
	byte *array;
	uint width;
	uint height;

protected:
	int index(uint x, uint y) const 
	{ 
		return y + width * x; 
	}
		
public:
	int alive;

	Map(uint w, uint h)
	{
		array = new byte[w * h]();
		alive = 0;
		width = w;
		height = h;
	}

	void cast(sf::Uint8 *pixels)
	{
		for (uint i = 0; i < height; i++)
		{
			for (uint j = 0; j < width; j++)
			{
				if (get(i, j) == 0)
				{
					pixels[(j + i * w) * 4 + 0] = 0;
					pixels[(j + i * w) * 4 + 1] = 0;
					pixels[(j + i * w) * 4 + 2] = 0;
					pixels[(j + i * w) * 4 + 3] = 255;
				}
				else
				{
					pixels[(j + i * w) * 4 + 0] = 255;
					pixels[(j + i * w) * 4 + 1] = 255;
					pixels[(j + i * w) * 4 + 2] = 255;
					pixels[(j + i * w) * 4 + 3] = 255;
				}
			}
		}
		return;
	}

	byte check(uint i, uint j)
	{
		uint iu = (i == 0) ? h - 1 : i - 1;
		uint id = (i == h - 1) ? 0 : i + 1;
		uint jl = (j == 0) ? w - 1 : j - 1;
		uint jr = (j == w - 1) ? 0 : j + 1;

		return get(iu, jl) + get(iu, j) + get(iu, jr) + get(i, jl) +
			get(i, jr) + get(id, jl) + get(id, j) + get(id, jr);
	}

	byte get(uint x, uint y) const
	{
		return array[index(x, y)];
	}

	void initialize()
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<uint> dist(0, 1);

		for (uint i = 0; i < height; i++)
		{
			for (uint j = 0; j < width; j++)
			{
				set(i, j, dist(mt));
				if (get(i, j) == 1)
					alive++;
			}
		}
		return;
	}

	void set(uint x, uint y, byte z) const
	{
		array[index(x, y)] = z;
		return;
	}

	~Map()
	{
		delete[] array;
	}
};

int main()
{
	int sleep = 0;
	int gens = 0;
	byte equal = 0;
	byte suspicious = 0;
	bool goteem = 0;
	bool doupdate = 1;
	bool doonce = 0;

	Map previous(w, h);

	Map current(w, h);

	Map next(w, h);
	
	sf::RenderWindow window(sf::VideoMode(W, H + 30, 8), "John Conway's Game of Life");

	sf::Texture texture;
	texture.create((int)w, (int)h);
	texture.setRepeated(false);

	sf::Sprite sprite;
	sprite.setPosition(0.0, 16.0);
	sprite.setScale((float)c, (float)c);
	sprite.setTexture(texture);

	sf::RectangleShape border;
	border.setFillColor(sf::Color::Transparent);
	border.setOutlineColor(sf::Color::White);
	border.setOutlineThickness(1.0);
	border.setPosition(0.0, 15.0);
	border.setSize(sf::Vector2f((float)W, (float)H + 2.0));

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/lucon.ttf"))
		return 0;

	sf::Text text;
	text.setCharacterSize(12);
	text.setColor(sf::Color::Green);
	text.setFont(font);
	text.setStyle(sf::Text::Bold);

	sf::Text text2;
	text2.setCharacterSize(12);
	text2.setColor(sf::Color::Red);
	text2.setFont(font);
	//text2.setPosition((float)W / 2.0 - 88.0, (float)H / 2.0);
	text2.setPosition((float)W - 176.0, 0.0);
	text2.setStyle(sf::Text::Bold);

	sf::Text text3;
	text3.setCharacterSize(12);
	text3.setColor(sf::Color::Green);
	text3.setFont(font);
	text3.setPosition(0.0, H + 17.0);
	text3.setString("Press Ctrl+R to Restart, Ctrl+P to Pause");
	text3.setStyle(sf::Text::Bold);

	sf::Uint8 *pixels = new sf::Uint8[w * h * 4];

	sf::Time t;

	//Main Cycle
	while (window.isOpen())
	{
		//Event Handing
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				return 0;
			}
		}

		//Gameplay Cycle
		if (doupdate)
		{
			//Map Managing
			if (!doonce)
			{
				current.initialize();
				doonce = 1;
			}
			else
			{
				current.alive = 0;
				for (uint i = 0; i < h; i++)
				{
					for (uint j = 0; j < w; j++)
					{
						byte hood = current.check(i, j);
						if (current.get(i, j) == 0)
						{
							if (hood == 3)
							{
								next.set(i, j, 1);
								next.alive += 1;
							}
							else
								next.set(i, j, 0);
						}
						else
						{
							if (hood == 2 || hood == 3)
							{
								next.set(i, j, 1);
								next.alive += 1;
							}
							else
								next.set(i, j, 0);
						}
					}
				}

				equal = 1;
				for (uint i = 0; i < h; i++)
				{
					for (uint j = 0; j < w; j++)
					{
						if (previous.get(i, j) != next.get(i, j))
							equal = 0;
					}
				}
				if (equal == 1)
					suspicious++;
				else if (suspicious > 1)
					suspicious = 0;
				equal = 0;

				if (suspicious == 2)
					goteem = 1;

				gens++;
				for (size_t i = 0; i < h; i++)
				{
					for (size_t j = 0; j < w; j++)
					{
						previous.set(i, j, current.get(i, j));
						current.set(i, j, next.get(i, j));
					}
				}
				previous.alive = current.alive;
				current.alive = next.alive;
			}

			//On Stasis Do The Following
			if (goteem)
			{
				doupdate = 0;

				//char buf[50];
				//sprintf(buf, "Stasis achieved @ %d", gens - 3);
				text2.setString("Statis achieved");

				sleep = 100;
				//write(current);
			}

			//Graphics Display
			current.cast(pixels);
			texture.update(pixels, w, h, 0, 0);

			//char buf[200];
			//sprintf(buf, "Generation: %d\tCells alive: %d", gens, current.alive);
			text.setString("Generation: ");

			window.clear();
			window.draw(sprite);
			window.draw(text);
			window.draw(text2);
			window.draw(text3);
			window.draw(border);
			window.display();
		}

		t = sf::milliseconds(sleep);
		sf::sleep(t);
	}

	delete pixels;

	return 0;
}