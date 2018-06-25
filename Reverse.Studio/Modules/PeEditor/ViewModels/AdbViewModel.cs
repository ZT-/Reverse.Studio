using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Gemini.Framework;
using Gemini.Framework.Services;
using Gemini.Modules.Output;

namespace Virtion.Knife.Modules.PeEditor.ViewModels
{

    [Export]
    public class AdbViewModel : Tool, IOutput
    {
        public override PaneLocation PreferredLocation { get { return PaneLocation.Bottom; } }
        public void AppendLine(string text)
        {
            throw new NotImplementedException();
        }

        public void Append(string text)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public TextWriter Writer { get; }
    }
}
