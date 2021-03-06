
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

/**********************************************
 * Highest Priority First (non-preemptive)
 * 
 * The highest priority process runs
 * until it is complete
 *
 * @author Team: Theta 1
 * CS149
 * 
 **********************************************/
public class HPF  implements SchedulerInterface{
    private static final int QUANTUM_MAX = 100;
	private ArrayList<Process> processList;
    private ArrayList<String> stringList;
    private ArrayList<Process> stats;
    
    public HPF(ArrayList<Process> processList) {
    	this.processList = processList;
    	stringList = new ArrayList<String>();
    	stats = new ArrayList<Process>();

    	run();
    }

    /**
     * Runs the HPF algorithm.
     */
    private void run() {
        // sorts Process by priority, then arrival time
        Comparator<Process> comparator = new Comparator<Process>() {
            public int compare(Process process1, Process process2) {
                int priorityDifference = process1.getPriority() - process2.getPriority();

                if(priorityDifference < 0) { // process1 has higher priority than process2
                    return -1;
                }
                else if(priorityDifference > 0) { // process2 has higher priority than process1
                    return 1;
                }
                else {
                    return new Float(process1.getArrivalTime()).compareTo(new Float(process2.getArrivalTime()));
                }
            }
        };

        ArrayList<Process> priorityProcessList = new ArrayList<>();
        priorityProcessList.add(processList.remove(0));
        int quantum = 0;

        while(quantum <= QUANTUM_MAX) {
            Process process = priorityProcessList.remove(0);
            //System.out.println("Current: " + process.getName() + "." + process.getPriority() + " ");

            // idle time
            while(process.getArrivalTime() > quantum) {
                stringList.add("");

                quantum++;
            }

            // process time
            process.setActualStartTime(quantum);
            while(process.getRunTime() > 0) {
                stringList.add(process.getName());

                // update process stats
                process.decrementRunTime();
                process.incrementQuantaTime();
                
                quantum++;
                
             // increased quantumWaitAmounts
                //System.out.print("Increased quantumWaitAmount: ");
                for(Process temp : priorityProcessList) {
                    // do not increase quantumWaitAmount for currently running process
                    if(temp != process) {
                        temp.incrementQuantumWaitAmount();
                        //System.out.print(temp.getName() + "." + temp.getPriority() + " ");
                    }
                }
            }
            process.setEndTime(quantum - 1);
            stats.add(process);

            // add new process into a the priorityList as a runTime has passed
            for(Process temp : processList) {
                if(temp.getArrivalTime() < quantum) {
                    priorityProcessList.add(temp);
                }
            }

            // make sure priorityProcessList has something in it
            if(priorityProcessList.isEmpty()) {
                priorityProcessList.add(processList.remove(0));
            }

            // remove all the elements in the priorityList from the processList
            processList.removeAll(priorityProcessList);

            // sort the priorityProcessList by priority, then arrivalTime
            Collections.sort(priorityProcessList, comparator);
            }
    }

    public ArrayList<String> getStringList() {
        return stringList;
    }

    public ArrayList<Process> getStats() {
	return stats;
    }
    
}
