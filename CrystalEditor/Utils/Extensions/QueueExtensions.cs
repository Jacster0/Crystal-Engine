using System.Collections.Concurrent;
using System.Collections.Generic;

namespace CrystalEditor.Utils.Extensions
{
    public static class QueueExtensions
    {
        public static bool Empty<T>(this Queue<T> queue)
        {
            return queue.Count == 0;
        }

        public static bool Empty<T>(this ConcurrentQueue<T> queue)
        {
            return queue.Count == 0;
        }
    }
}
