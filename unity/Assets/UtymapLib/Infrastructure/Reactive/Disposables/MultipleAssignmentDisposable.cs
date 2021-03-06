﻿using System;
using System.Collections;

namespace Assets.UtymapLib.Infrastructure.Reactive
{
    public class MultipleAssignmentDisposable : IDisposable, ICancelable
    {
        static readonly BooleanDisposable True = new BooleanDisposable(true);

        object gate = new object();
        IDisposable current;

        public bool IsDisposed
        {
            get
            {
                lock (gate)
                {
                    return current == True;
                }
            }
        }

        public IDisposable Disposable
        {
            get
            {
                lock (gate)
                {
                    return (current == True)
                        ? Assets.UtymapLib.Infrastructure.Reactive.Disposable.Empty
                        : current;
                }
            }
            set
            {
                var shouldDispose = false;
                lock (gate)
                {
                    shouldDispose = (current == True);
                    if (!shouldDispose)
                    {
                        current = value;
                    }
                }
                if (shouldDispose && value != null)
                {
                    value.Dispose();
                }
            }
        }

        public void Dispose()
        {
            IDisposable old = null;

            lock (gate)
            {
                if (current != True)
                {
                    old = current;
                    current = True;
                }
            }

            if (old != null) old.Dispose();
        }
    }
}