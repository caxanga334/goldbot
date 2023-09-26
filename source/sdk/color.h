#ifndef SDK_COLORS_H_
#define SDK_COLORS_H_

class Color
{
public:
	Color()
	{
		m_colors[0] = 255;
		m_colors[1] = 255;
		m_colors[2] = 255;
		m_alpha = 255;
	}

	Color(int red, int green, int blue)
	{
		m_colors[0] = red;
		m_colors[1] = green;
		m_colors[2] = blue;
		m_alpha = 255;
	}

	Color(int red, int green, int blue, int alpha)
	{
		m_colors[0] = red;
		m_colors[1] = green;
		m_colors[2] = blue;
		m_alpha = alpha;
	}

	int GetRed() { return m_colors[0]; }
	int GetGreen() { return m_colors[1]; }
	int GetBlue() { return m_colors[2]; }
	int GetAlpha() { return m_alpha; }

private:
	int m_colors[3];
	int m_alpha;
};

#endif // !SDK_COLORS_H_

