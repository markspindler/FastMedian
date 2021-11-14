/**
FastMedian v1.1.0 (c) by Mark Spindler

FastMedian is licensed under a Creative Commons Attribution 3.0 Unported License.

To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/.
**/


static const char* const CLASS = "FastMedian";
static const char* const HELP =	"Performes the same filter as Nuke's own Median node, but in a shorter render time.\n\n"

								"Version: 1.1.0\n"
								"Author: Mark Spindler\n"
								"Contact: info@mark-spindler.com";


#include "DDImage/Iop.h"
#include "DDImage/NukeWrapper.h"
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"



using namespace DD::Image;



class FastMedian : public Iop
{
	private:
		float _size;
		int size_int;
		int amount;

	public:
		int maximum_inputs() const {return 1;}
		int minimum_inputs() const {return 1;}

		FastMedian (Node* node) : Iop (node)
		{
			_size = 0;
		}

		~FastMedian ()
		{}

		void knobs(Knob_Callback);
		void _validate(bool);
		void _request(int x, int y, int r, int t, ChannelMask channels, int count);
		void engine(int y, int x, int r, ChannelMask channels, Row& out);

		const char* Class() const {return CLASS;}
		const char* node_help() const {return HELP;}
		static const Iop::Description description;
};


void FastMedian::knobs(Knob_Callback f)
{
	Float_knob(f, &_size, "size");
	SetRange(f, 0, 10);
}


void FastMedian::_validate(bool for_real)
{
	size_int = std::max((float)ceil((float)outputContext().to_proxy_x(_size)), 0.0f);
	amount = pow((1 + size_int*2), 2.0f);

	copy_info();
}


void FastMedian::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
	input(0)->request(x - size_int, y - size_int, r + size_int, t + size_int, channels, count);
}


void FastMedian::engine(int y, int x, int r, ChannelMask channels, Row& row)
{
	Tile tile(input0(), x - size_int, y - size_int, r + size_int, y + size_int + 1, channels);
	if (aborted())
	{
		std::cerr << "Aborted!";
		return;
	}

	foreach (z, channels) 
	{
		float* outptr = row.writable(z) + x;

		for(int cur_x = x ; cur_x < r; cur_x++)
		{
			//collect all values within radius and save them in list "values"
			float* values = new float[amount];
			int i = 0;

			if (intersect(tile.channels(), z)) 
			{
				for (int px = -size_int; px <= size_int; px++)
				{
					for (int py = -size_int; py <= size_int; py++)
					{ 
						values[i] = tile[z][tile.clampy(y + py)][tile.clampx(cur_x + px)];
						i++;
					}
				}
			}

			//get median value of list and assign value to out-pointer
			std::sort(values, values + amount);
			float value = values[amount / 2];

			delete[] values;

			*outptr++ = value;
		}
	}
}


static Iop* FastMedianCreate(Node* node)
{
	return (new NukeWrapper(new FastMedian(node)))->channelsRGBoptionalAlpha();
}


const Iop::Description FastMedian::description (CLASS, "FastMedian", FastMedianCreate);