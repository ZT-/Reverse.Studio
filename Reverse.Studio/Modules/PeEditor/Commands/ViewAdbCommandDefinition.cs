using Gemini.Framework.Commands;

namespace Virtion.Knife.Modules.PeEditor.Commands
{
    [CommandDefinition]
    public class ViewAdbCommandDefinition : CommandDefinition
    {
        public const string CommandName = "View.Adb";

        public override string Name
        {
            get { return CommandName; }
        }

        public override string Text
        {
            get { return "Adb"; }
        }

        public override string ToolTip
        {
            get { return "Adb"; }
        }
    }
}