package appGui;

import appGui.util.FSBrowser;
import appModel.GeneralOption;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.input.MouseEvent;

public class GeneralOptionsCtrl
{
	@FXML private Label workingDirectoryLabel;
	
	private GeneralOption m_generalOption;
	
    @FXML
    public void initialize()
    {
    	m_generalOption = null;
    }
    
    @FXML
    void workingDirectoryBrowseBtnClicked(MouseEvent event)
    {
    	FSBrowser browser = new FSBrowser(workingDirectoryLabel.getScene().getWindow());
    	browser.setBrowserTitle("Select Working Directory");
		browser.startBrowsingDirectory();
		
		String directoryAbsPath = browser.getSelectedDirectoryAbsPath();
		if(directoryAbsPath != "")
		{
			m_generalOption.set(GeneralOption.WORKING_DIRECTORY, directoryAbsPath);
			
			setWorkingDirectoryLabel(directoryAbsPath);
		}
    }
    
    @FXML
    void saveBtnClicked(MouseEvent event)
    {
    	m_generalOption.save();
    }
    
    @FXML
    void loadDefaultsBtnClicked(MouseEvent event)
    {
    	// FIXME: broken
//    	m_generalOption.setToDefaults();
    	
    	setWorkingDirectoryLabel(m_generalOption.get(GeneralOption.WORKING_DIRECTORY));
    }
    
    public void setGeneralOption(GeneralOption generalOption)
    {
    	m_generalOption = generalOption;
    	
    	setWorkingDirectoryLabel(generalOption.get(GeneralOption.WORKING_DIRECTORY));
    }
    
    private void setWorkingDirectoryLabel(String labelText)
    {
    	workingDirectoryLabel.setText(labelText);
    	workingDirectoryLabel.getTooltip().setText(labelText);
    }
}
