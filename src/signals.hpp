#ifndef SIGNALS_H
#define SIGNALS_H

// enum of the signals which can be sent to the `signalQueue'

enum Signals {
  StrokeStarted
  , StrokeFinished
  , UndoRequested
  , RedoRequested
};

#endif /* SIGNALS_H */
