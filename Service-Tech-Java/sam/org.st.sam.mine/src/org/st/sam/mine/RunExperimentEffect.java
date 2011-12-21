package org.st.sam.mine;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XLog;
import org.st.sam.log.SLog;
import org.st.sam.log.SLogTree;
import org.st.sam.log.SLogTreeNode;
import org.st.sam.log.SScenario;
import org.st.sam.log.XESImport;
import org.st.sam.mine.MineLSC.Configuration;

import com.google.gwt.dev.util.collect.HashMap;

public class RunExperimentEffect extends RunExperimentCompare {

  private static String SLASH = System.getProperty("file.separator");
  
  public RunExperimentEffect() throws IOException {
    super();
  }
  
  public RunExperimentEffect(String dotBinary, String mscGenBinary) throws IOException {
    super(dotBinary, mscGenBinary);
  }

  
  public List<short[]> getCanonicalEffects(String logFile) throws IOException {
    List<short[]> triggers = new LinkedList<short[]>();
    
    XESImport xin = new XESImport();
    XLog xlog = xin.readLog(logFile);
    SLog log = new SLog(xlog);
    SLogTree tree = new SLogTree(log, true);
    
    // add all leaf events to the effects
    for (SLogTreeNode n : tree.nodes) {
      if (n.post.length == 0) {
        triggers.add(new short[] { n.id } );  
      }
    }
    
    return triggers;
  }
  
  @Override
  public void runMiners(String logFile, int minSupportThreshold, double confidence) throws IOException {
    List<short[]> effects = getCanonicalEffects(logFile);
    
    Configuration c_br = Configuration.mineBranching();
    c_br.effects = effects;
    minerBranch = new MineLSC(c_br);
    minerBranch.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining branching lscs from "+logFile);
    minerBranch.mineLSCs(logFile, minSupportThreshold, confidence);
    
    Configuration c_lin = Configuration.mineLinear();
    c_lin.effects = effects;
    minerLinear = new MineLSC(c_lin, minerBranch.getSupportedWords());
    minerLinear.OPTIONS_WEIGHTED_OCCURRENCE = true;
    System.out.println("mining linear lscs from "+logFile);
    //LSCEvent[][] dataSet = MineLinearLSC.readInput(experimentFileRoot+"/"+inputFile);
    //if (dataSet == null)
    //  return;
    //LSCMiner minerLinear = new LSCMiner();
    //minerLinear.mineLSCs(dataSet, support, confidence, density);
    minerLinear.mineLSCs(logFile, minSupportThreshold, confidence);
    
    originalScenarios = new HashMap<LSC, SScenario>();
    for (LSC l : minerBranch.getScenarios().keySet()) {
      originalScenarios.put(l, minerBranch.getScenarios().get(l));
    }
    for (LSC l : minerLinear.getScenarios().keySet()) {
      originalScenarios.put(l, minerLinear.getScenarios().get(l));
    }  
  }
  
  @Override
  public String getResultsDirName(String dir, String logFileName, int minSupportThreshold, double confidence) {

    DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss");
    return dir+SLASH+"results_"+logFileName+"_EFF_"+minSupportThreshold+"_"+confidence+"_"+dateFormat.format(now);
  
  }
  
  @Override
  protected void writeResults_perPreChart(StringBuilder r,
      LSCEvent[] preChart,
      final String resultsDir,
      int treeFigHeight,
      boolean render_trees) throws IOException
  {
    if (minerBranch == null) render_trees = false;

    String title = "";
    for (LSCEvent e : preChart) title += " "+e.getMethod();
    
    ArrayList<LSC> both_match = new ArrayList<LSC>();
    ArrayList<LSC> branching_match = new ArrayList<LSC>();
    ArrayList<LSC> linear_match = new ArrayList<LSC>();
    
    for (LSC l : both) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          /*&& !isSubsumed(l, minerBranch, originalScenarios)*/)
      {
        both_match.add(l);
        System.out.println(originalScenarios.get(l));
      }
    }
    for (LSC l : onlyLinear) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          /*&& !isSubsumed(l, minerBranch, originalScenarios)*/)
      {
        //linear_match.add(l);
        // strictly linear scenarios (not discovered by branching miner)
        // are subsumed by branching scenarios by construction, i.e. they
        // are also discovered by the branching miner: add to both_match
        both_match.add(l);
      }
    }
    for (LSC l : onlyBranching) {
      if (pre_chart_comp.compare(l.getPreChart(), preChart) == 0
          /*&& !isSubsumed(l, minerBranch, originalScenarios)*/)
      {
        branching_match.add(l);
      }
    }
    
    if (branching_match.size() == 0 && both_match.size() == 0) return;
    
    r.append("<h2>"+title+"</h2>\n");
    if (branching_match.size() > 0) {
      r.append("<b>"+branching_match.size()+" strict branching LSC</b></br>");
      appendResults(r, branching_match, resultsDir, treeFigHeight, render_trees,
          "strictly branching LSC",
          "lsc_branch");
    }
    if (both_match.size() > 0) {
      r.append("<b>"+both_match.size()+" linear and branching LSC</b></br>");
      appendResults(r, both_match, resultsDir, treeFigHeight, render_trees,
          "linear and branching LSC",
          "lsc_both");
    }
    
    r.append("<a href='#general'>top</a><br/>\n");
    r.append("<hr/>\n");
  }
  
  public static void main(String[] args) throws IOException {
    
    RunExperimentEffect exp = new RunExperimentEffect();
    if (!exp.readCommandLine(args)) return;
    exp.experiment();
  }

}