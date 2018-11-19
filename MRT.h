/*
Copyright (C) 2011 Georgia Institute of Technology

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
* Generates square pulse current commands.
*/

#include <default_gui_model.h>
#include <string>

class MRT : public DefaultGUIModel {
	
	public:
		MRT(void);
		virtual
		~MRT(void);
		virtual void
		execute(void);
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
	
	private:
		#define EPS 1e-9
		double V, Iout, Output_to_mV;

		double dt;
		double period;
		double rStart, rEnd;
		int step_size;
		int Ncycles;
		int cycle;
		double maxAmp;
		double ICI;
		double Downtime;
		double deltaI;
		double step, age, interage, time;	
};
