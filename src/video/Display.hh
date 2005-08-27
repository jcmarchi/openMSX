// $Id$

#ifndef DISPLAY_HH
#define DISPLAY_HH

#include "Layer.hh"
#include "RendererFactory.hh"
#include "EventListener.hh"
#include "SettingListener.hh"
#include "Command.hh"
#include "InfoTopic.hh"
#include "CircularBuffer.hh"
#include "Alarm.hh"
#include "openmsx.hh"
#include <memory>
#include <string>
#include <vector>

namespace openmsx {

class VideoSystem;
class UserInputEventDistributor;
class RenderSettings;
class Console;

/** Represents the output window/screen of openMSX.
  * A display contains several layers.
  */
class Display : private EventListener, private SettingListener,
                private LayerListener
{
public:
	Display(UserInputEventDistributor& userInputEventDistributor,
	        RenderSettings& renderSettings,
	        Console& console);
	virtual ~Display();

	VideoSystem& getVideoSystem();
	void resetVideoSystem();
	void setVideoSystem(VideoSystem* videoSystem);

	/** Redraw the display.
	  */
	void repaint();
	void repaintDelayed(unsigned long long delta);

	void addLayer(Layer* layer);

private:
	// EventListener interface
	virtual void signalEvent(const Event& event);
	
	// SettingListener interface
	virtual void update(const Setting* setting);

	void checkRendererSwitch();

	typedef std::vector<Layer*> Layers;

	/** Find frontmost opaque layer.
	  */
	Layers::iterator baseLayer();

	// LayerListener interface
	virtual void updateCoverage(Layer* layer, Layer::Coverage coverage);
	virtual void updateZ(Layer* layer, Layer::ZIndex z);

	Layers layers;
	std::auto_ptr<VideoSystem> videoSystem;

	// the current renderer
	RendererFactory::RendererID currentRenderer;
	
	// fps related data
	static const unsigned NUM_FRAME_DURATIONS = 50;
	CircularBuffer<unsigned long long, NUM_FRAME_DURATIONS> frameDurations;
	unsigned long long frameDurationSum;
	unsigned long long prevTimeStamp;

	// Delayed repaint stuff
	class RepaintAlarm : public Alarm {
	private:
		virtual void alarm();
	} alarm;

	// Commands
	class ScreenShotCmd : public SimpleCommand {
	public:
		ScreenShotCmd(Display& display);
		virtual std::string execute(const std::vector<std::string>& tokens);
		virtual std::string help(const std::vector<std::string>& tokens) const;
	private:
		Display& display;
	} screenShotCmd;

	// Info
	class FpsInfoTopic : public InfoTopic {
	public:
		FpsInfoTopic(Display& display);
		virtual void execute(const std::vector<TclObject*>& tokens,
		                     TclObject& result) const;
		virtual std::string help(const std::vector<std::string>& tokens) const;
	private:
		Display& display;
	} fpsInfo;

	UserInputEventDistributor& userInputEventDistributor;
	RenderSettings& renderSettings;
	Console& console;
};

} // namespace openmsx

#endif
