//
//  quietMode.swift
//  Shrine
//
//  Created by Zaid Aggad on 09/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit
import MaterialComponents
import MessageUI

class quietMode: UIViewController, MFMessageComposeViewControllerDelegate {
    func messageComposeViewController(_ controller: MFMessageComposeViewController, didFinishWith result: MessageComposeResult) {
        controller.dismiss(animated: true, completion: nil)
    }
    
    var appBarViewController = MDCAppBarViewController()
    
    @IBOutlet weak var clock: AnalogClockView!
    override func viewDidLoad() {
        super.viewDidLoad()
        super.viewDidLoad()
        super.viewDidLoad()
        self.view.backgroundColor = GMColor.imperialblue()
        clock.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.view.tintColor = .white
        let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
        self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])
        self.title = "Quiet Mode"
        self.addChildViewController(self.appBarViewController)
        self.view.addSubview(self.appBarViewController.view)
        self.appBarViewController.didMove(toParentViewController: self)
        
        let menuItemImage = UIImage(named: "MenuItem")
        let templatedMenuItemImage = menuItemImage?.withRenderingMode(.alwaysTemplate)
        let menuItem = UIBarButtonItem(image: templatedMenuItemImage,
                                       style: .plain,
                                       target: self,
                                       action:  #selector(menuItemTapped(sender:)))
        self.navigationItem.leftBarButtonItem = menuItem

        // Do any additional setup after loading the view.
    }
    @objc func menuItemTapped(sender: Any) {
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromLeft
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.dismiss(animated: false, completion: nil)
    }

    @IBAction func hide(_ sender: Any) {
        if self.appBarViewController.navigationBar.isHidden == false{
            self.appBarViewController.navigationBar.isHidden = true
            self.appBarViewController.headerView.isHidden = true
            UIApplication.shared.keyWindow?.windowLevel = UIWindowLevelStatusBar
        }
        else{
             self.appBarViewController.navigationBar.isHidden = false
             self.appBarViewController.headerView.isHidden = false
            UIApplication.shared.keyWindow?.windowLevel = UIWindowLevelNormal
        }
    }
    @IBAction func sosb(_ sender: Any) {
        displayMessageInterface()
    }
    
    
    func displayMessageInterface() {
        let composeVC = MFMessageComposeViewController()
        composeVC.messageComposeDelegate = self
        
        // Configure the fields of the interface.
        let x = UserDefaults.standard.object(forKey: Phonekey) as? String
        composeVC.recipients = [x!]
        composeVC.body = "I Need Help!"
        
        // Present the view controller modally.
        if MFMessageComposeViewController.canSendText() {
            self.present(composeVC, animated: true, completion: nil)
        } else {
            print("Can't send messages.")
        }
    }
    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
