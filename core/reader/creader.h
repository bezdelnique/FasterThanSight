#pragma once

#include "../textfragment/ctextfragment.h"

DISABLE_COMPILER_WARNINGS
#include <QTimer>
RESTORE_COMPILER_WARNINGS

#include <vector>

class CReader
{
public:
	struct TextFragmentWithPause {
		TextFragment _textFragment;
		size_t _pauseAfter;
	};

	enum State {Reading, Paused};

	struct ReaderInterface {
		virtual void updateDisplay(const size_t currentTextFragmentIndex) = 0;
		virtual void stateChanged(const State newState) = 0;
	};

	explicit CReader(ReaderInterface* interface);

// Preparation
	void load(const std::vector<TextFragment>& textFragments);
	bool loadFromFile(const QString& filePath);

	void setClearScreenAfterSentenceEnd(bool clear);
	void setLongWordPauseScaling(unsigned lengthThreshold, float scaleFactor);

// State
	State state() const;
	QString filePath() const;
	// Current word index
	size_t position() const;
	size_t totalNumWords() const;
	long double progress() const; // [0.0; 1.0]
	size_t timeRemainingSeconds() const;

	// Notify the client of current reader state via ReaderInterface
	void updateInterface() const;

// Data
	inline const TextFragmentWithPause& textFragment(const size_t fragmentIndex) const {
		if (fragmentIndex >= _textFragments.size())
		{
			static const TextFragmentWithPause emptyFragment {TextFragment(), 0};
			return emptyFragment;
		}
		else
			return _textFragments[fragmentIndex];
	}

// Control
	void resumeReading();
	void pauseReading();
	void togglePause();

	void resetAndStop();
	// Specify 0-based word index to go to
	void goToWord(size_t wordIndex);

// Settings
	size_t readingSpeed() const;
	void setReadingSpeed(size_t wpm);

private:
	void readNextFragment();

	// Calculates the pause after the specific fragment in ms
	size_t pauseForFragment(const TextFragment& fragment) const;
	// Recalculate all the pauses for the entire text
	void updatePauseValues();

private:
	ReaderInterface * const _interface;

	QString _filePath;

	std::vector<TextFragmentWithPause> _textFragments;
	size_t                    _position = 0;
	size_t                    _speedWpm = 250;

	State _state = Paused;

	QTimer _readingTimer;

// Pause-related stuff
	bool _clearScreenAfterSentenceEnd = false;
	unsigned _longWordThreshold = 7;
	float _longWordDelayScaleFactor = 1.1f;
};
