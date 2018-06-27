using System.ComponentModel.Composition;
using System.Threading.Tasks;
using Caliburn.Micro;
using Gemini.Framework;
using Gemini.Framework.Commands;
using Gemini.Framework.Services;
using Gemini.Framework.Threading;
using Gemini.Modules.Output;
using Virtion.Knife.Modules.PeEditor.ViewModels;

namespace Virtion.Knife.Modules.PeEditor.Commands
{
    [CommandHandler]
    public class ViewAdbCommandHandler : CommandHandlerBase<ViewAdbCommandDefinition>
    {
        private readonly IShell _shell;

        [ImportingConstructor]
        public ViewAdbCommandHandler(IShell shell)
        {
            _shell = shell;
        }

        public override Task Run(Command command)
        {
            _shell.ShowTool(IoC.GetInstance(typeof(AdbViewModel), null) as IOutput);
            return TaskUtility.Completed;
        }
    }
}